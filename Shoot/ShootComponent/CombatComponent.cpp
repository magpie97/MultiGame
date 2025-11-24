// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/HUD/ShooterHUD.h"
#include "GameFramework/HUD.h"
#include "Camera/CameraComponent.h"
//#include "TimerManager.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Sound/SoundCue.h"
#include "Shoot/Weapon/Projectile.h"
#include "Shoot/Weapon/ProjectileGrenade.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraShakeBase.h"
#include "Components/AudioComponent.h" 


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//bReplicates = true;

	BaseWalkSpeed = 450.f;
	AimWalkSpeed = 330.f;

	// testing value 650.f
	SprintSpeed = 650.f;	

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 무기 등록  ( 따라서 이제 장착된 무기가 복제 된다  즉, 무기가 변경되면 해당 변경 사항이 모든 클라이언트에 반영된다)
	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, bSprint);
	DOREPLIFETIME(UCombatComponent, bJump);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);	// condition을 사용했을때 클라이언트에만 복제를 하기때문에 서버에 알릴 필요없이 복사를 하며 대역폭을 줄이는 최적화 기능이다
	DOREPLIFETIME(UCombatComponent, CombatState);
	//DOREPLIFETIME(UCombatComponent, bGrenadeHold);
	DOREPLIFETIME(UCombatComponent, CarriedGrenade);
	DOREPLIFETIME(UCombatComponent, bCanFire);



}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;


			// 게임 시작시 first 카메라 위치를 저장
			DefaultCameraLocation = Character->GetFollowCamera()->GetRelativeLocation();
			// 저장한 first 카메라 위치를 현재 위치 로 저장
			CurrentCameraLocation = DefaultCameraLocation;
		}

		// rifle 기본 탄약 30으로 초기화  서버인지 확인
		if (Character->HasAuthority())
		{
			InitCarriedAmmo();
		}
	}
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SetHUDCrosshairs(DeltaTime);

	if (Character && Character->IsLocallyControlled())
	{
		// 총구가 애님인스턴스에 구현된 거리에 탄착점 위치가 맞다면 tick 함수에 실행
		FHitResult HitResult;
		TraceUnderCrosshair(HitResult);
		HitTarget = HitResult.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		
		InterpFOV(DeltaTime);

		InterpCamera(DeltaTime);
		
	}
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr && Character->Controller == nullptr) return;

	// crosshair의 동적으로 사용하기 위해 playercontroller 클래스를 사용하였고 
	// combat component 클래스에서 crosshair를 처리하기 위해 character의 controller를 cast한다  입력을 받아서 움직여야하는 crosshair이기 때문  
	Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;

	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<AShooterHUD>(Controller->GetHUD()) : HUD;
	}

	if (HUD)
	{
		FHUDCrosshairs HUDCrosshair;

		if (EquippedWeapon)
		{
			HUDCrosshair.FHC_CrosshairCenter = EquippedWeapon->CrosshairCanter;
			HUDCrosshair.FHC_CrosshairLeft = EquippedWeapon->CrosshairLeft;
			HUDCrosshair.FHC_CrosshairRight = EquippedWeapon->CrosshairRight;
			HUDCrosshair.FHC_CrosshairUp = EquippedWeapon->CrosshairUp;
			HUDCrosshair.FHC_CrosshairDown = EquippedWeapon->CrosshairDown;
		}
		else
		{
			HUDCrosshair.FHC_CrosshairCenter = nullptr;
			HUDCrosshair.FHC_CrosshairLeft = nullptr;
			HUDCrosshair.FHC_CrosshairRight = nullptr;
			HUDCrosshair.FHC_CrosshairUp = nullptr;
			HUDCrosshair.FHC_CrosshairDown = nullptr;
		}
		HUD->SetHUDCrosshairs(HUDCrosshair);
	}
}



void UCombatComponent::SetAiming(bool bIsAiming)	// deltatime test 
{
	/*Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	Character->bUseControllerRotationYaw = true;*/

	// bAiming 변수는 복사된 변수이기 때문에 모든 클라이언트는 서버의 aiming 애니메이션을 볼수 있었다 따라서 
	//  combatcomponenet에 있는 복사할 변수인 bAming에  setaiming함수 매개변수인 bIsAiming 을 set 시킨다
	bAiming = bIsAiming;

	

	ServerSetAiming(bAiming);

	//float deltatime = GetWorld()->GetDeltaSeconds();

	//Character->GetCharacterMovement()->bOrientRotationToMovement = false;	// default = false
	//Character->bUseControllerRotationYaw = true; // default  = true


	if (Character && bAiming)
	{
		UGameplayStatics::PlaySound2D(this, AimingSound);
		Character->GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;	//test bIsAiming
	
	}
	else
	{

		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}

	// 클라이언트에 aiming animation을 복사 하지 않고 빨리 보여주기 위한 작업
	if (Character->IsLocallyControlled())
	{
		bAimingButtonPressed = bIsAiming;

	}
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)		// deltatime test 
{
	bAiming = bIsAiming;

	//float deltatime = GetWorld()->GetDeltaSeconds();

	//Character->GetCharacterMovement()->bOrientRotationToMovement = false;	// default = false
	//Character->bUseControllerRotationYaw = true; // default  = true

	// default
		// default
	if (Character && bAiming)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;	//test bIsAiming

	}
	else
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}
}

void UCombatComponent::OnRep_Aiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		//UE_LOG(LogTemp, Error, TEXT("OnRep_Aiming functions access"));

		bAiming = bAimingButtonPressed;

	}

}

void UCombatComponent::SetSprint(bool bIsSprint)
{
	bSprint = bIsSprint;
	
	ServerSetSprint(bSprint);

	
	if (Character && bSprint)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		
	}
	else
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}

	
	
}

void UCombatComponent::ServerSetSprint_Implementation(bool bIsSprint)
{
	bSprint = bIsSprint;

	if (Character && bSprint)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	}
	else
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	}

}


void UCombatComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;

	if (bFireButtonPressed)
	{
		// LineTrace in Tickfunction  
		/*FHitResult HitResult;
		TraceUnderCrosshair(HitResult);*/
		Fire();

	}
}

void UCombatComponent::Fire()
{
	// 무기 소지와 발사할수있다면
	if (CanFire() && EquippedWeapon)		
	{
		// fire 함수를 계속 콜 할수 없도로 false로 초기화
		bCanFire = false;


		if (EquippedWeapon)
		{
			switch (EquippedWeapon->FireType)
			{
			case EFireType::EFT_Projectile :
				FireProjectileWeapon();

				break;
			}
		}

		// 카메라 쉐이크
		if (CameraShake)
		{
			GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CameraShake);
		}

		// 총을 연사 딜레이 
		StartFireTimer();
	}
}

void UCombatComponent::FireProjectileWeapon()
{

	if (EquippedWeapon && Character)
	{
		if (!Character->HasAuthority())
		{
			LocalFire(HitTarget);
		}
		ServerSetFire(HitTarget, EquippedWeapon->FireDelay);

	}
	
	// test code
	/*if (CanFire())
	{
		ServerSetFire(HitTarget);

		if (Character && !Character->HasAuthority())
		{
			LocalFire(HitTarget);
		}
		StartFireTimer();
	}*/




}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr && Character == nullptr) return;

	Character->GetWorldTimerManager().SetTimer(FireTimeHandle, this, &UCombatComponent::EndFireTimer, EquippedWeapon->FireDelay);
	
}

void UCombatComponent::EndFireTimer()
{
	if (EquippedWeapon == nullptr) return;

	bCanFire = true; //test 

	Character->GetWorldTimerManager().ClearTimer(FireTimeHandle);

	// 버튼이 true 라면
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}

	// 탄약 0 일때 재장전
	ReloadWeapon();

}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;

	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::ServerSetFire_Implementation(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	MulticastSetFire(TraceHitTarget);

}

// 유효성 검사  weapon 클래스에 비 이상적인 값으로 초기화 하여 게임 진행에 있어 불합리한 상황이 생길 우려를 조기에 진압하여 처리할 함수
// fire delay 만 처리하여 테스트 진행 후 결과는 세션을 강제 퇴장 시켜 더 이상 게임을 진행하지 못하게 처리를 하게됨 
bool UCombatComponent::ServerSetFire_Validate(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	if (EquippedWeapon)
	{
		bool FireDelayEqualValue = FMath::IsNearlyEqual(EquippedWeapon->FireDelay, FireDelay, 0.001f);

		return FireDelayEqualValue;
	}
	return true;

}

void UCombatComponent::MulticastSetFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;

	LocalFire(TraceHitTarget);

}

void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;

	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);

		

		EquippedWeapon->Fire(TraceHitTarget);

	}

}

void UCombatComponent::TraceUnderCrosshair(FHitResult& TraceHitResult)
{
	// 내가 캐릭터의 카메로라 바라보는 뷰포트 중앙의 값을 얻어야 캐릭터와 내가 바라보는 위치를 얻어낼 수 있다
	FVector2D ViewPortSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

	//GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Blue, TEXT("tracing crosshair")); // test

	// 뷰포트 사이즈의 중간위치 값 초기화
	FVector2D CrosshairLocation(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		/*
		캐릭터가 무기 장착 후 바라보는 방향이 전방인데 deprojectscreentoworld 함수로 2d 에서 3d 공간 위치값을 얻을 수 있는 함수로 인해  캐릭터가 벽 옆으로 서 viewport의 중간 위치점을 벽으로 바라 봤을때
		장착 된 무기의 방향이 캐릭터의 뒤쪽으로 돌아가는 문제점이 생겼다 이를 해결 하기 위해 
		캐릭터의 위치값과 CrosshairWorldPosition의 값을 복사한 Start 변수값을 뺀 크기를 저장한다  (캐릭터 위치값 부터 바라보는 viewprot 위치값 까지의 size값을 저장한다)
		viewport 중간점 위치 한 3D position 값을 저장한 Start 변수에  (Start + (조준점 방향값 + (캐릭터 위치값부터 Viewport 끝점까지의 거리값 + 100.f))를 저장한다

		이리하면 캐릭터의 전방 방향으로만 바라보게 되며  다른 모든 collision이 포함된 액터나 캐릭터에도 반응하지 않는다
		*/

		float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 150.f);		// 150.f default value
		//DrawDebugSphere(GetWorld(), Start, 12.f, 0, FColor::Red, false);
		
		// end 
		FVector End = Start + CrosshairWorldDirection * 10000000.f;
		//DrawDebugSphere(GetWorld(), End, 12, 0, FColor::Red, false);
		
		GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

		
		if (!TraceHitResult.bBlockingHit) 
		{
			//UE_LOG(LogTemp, Warning, TEXT("is not blocking hit"));

			TraceHitResult.ImpactPoint = End;
		}
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponTypes::EWT_Pistol, StartPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponTypes::EWT_Rifle, StartRifleAmmo);
	CarriedAmmoMap.Emplace(EWeaponTypes::EWT_Grenade, StartGrenade);
	
	
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState == ECombatState::ECS_Unoccupied && EquippedWeapon && !EquippedWeapon->IsFullAmmo())
	{
		ServerReload();
		HandleReload();
	}
	
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_Reloading;
	
	// 클라이언트에서 재 장전을할때 서버에서 애니메이션실행, CombatState가 복제되고  애니메이션 실행이 되어 두번 실행되는 문제가 있다 
	// 그래서 플레이어의 컨트롤러가 서버가 아닐때 재장전 애니메이션을 실행해야 두번 실행 되는 모습을 방지한다
	if (!Character->IsLocallyControlled())
	{
		HandleReload();
	}
	

}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case(ECombatState::ECS_Reloading):
		if (Character && !Character->IsLocallyControlled()) HandleReload();
		break;

	case(ECombatState::ECS_Unoccupied):
		if (bFireButtonPressed && !Character->HasAuthority())
		{
			bCanThrowGrenade = false;  //test
			Fire();

		}
		break;
	case(ECombatState::ECS_ThrowGrenadeHold):
		if (bCanThrowGrenade && Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeHoldMontage();
			ShowGrenadeMesh(true);

			

		}
		break;

	case(ECombatState::ECS_ThrowGrenade):
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();
			ShowGrenadeMesh(true);
			
		}
		break;

	}
}

void UCombatComponent::HandleReload()
{
	if (Character)
	{
		Character->PlayReloadMontage();
	}
	

}

void UCombatComponent::FinishReload()
{
	if (Character == nullptr) return;
	//Character = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;	// 테스트 코드
	if (Character->HasAuthority())	// 내가 서버인지 확인 참이면 true
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValue();
	}

	if (bFireButtonPressed)
	{
		Fire();
	}

}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;

	// 탄창용량의 값 - 현재 총알의 값을 뺀 값을 초기화
	int32 SpaceInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();

	// EWeaponTypes의 값이 있다면 참
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponTypes()))
	{
		// map의 value 의 값을 초기화
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponTypes()];

		// 최소한의 총알
		int32 Least = FMath::Min(SpaceInMag, AmountCarried);

		return FMath::Clamp(SpaceInMag, 0, Least);

	}
	
	return 0;
}

void UCombatComponent::UpdateAmmoValue()
{
	if (Character == nullptr && EquippedWeapon == nullptr) return;

	int32 ReloadAmount = AmountToReload();

	// EWeaponTypes의 값이 있다면 참  
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponTypes())) // bool 타입
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponTypes()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponTypes()];
	}

	Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;

	if (Controller)	// 서버인지 체크는 테스트 코드임
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AddAmmo(ReloadAmount);

}

void UCombatComponent::OnRep_CarriedGrenade()
{
	UpdateCarriedGrenade();

}

void UCombatComponent::ThrowGrenadeHold(bool bGrenade)
{
	bCanThrowGrenade = bGrenade;

	//class ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwner());


	if (CarriedGrenade == 0) return;

	if (Character == nullptr) return;
	
	if (CarriedGrenade <= 0)
	{
		bCanThrowGrenade = false;
		
	}
	else
	{ 
		CombatState = ECombatState::ECS_ThrowGrenadeHold;
		ShowGrenadeMesh(true);

		if (Character->IsLocallyControlled())
		{
			bCanFire = false;
		}
		bCanFire = false;

		if (Character && bCanThrowGrenade)
		{
			ServerThrowGrenadeHold();
		}
	}
}

void UCombatComponent::ServerThrowGrenadeHold_Implementation()
{
	MultiThrowGrenadeHold();

}

void UCombatComponent::MultiThrowGrenadeHold_Implementation()
{
	if (Character == nullptr) return;

	CombatState = ECombatState::ECS_ThrowGrenadeHold;
	ShowGrenadeMesh(true);
	bCanFire = false;

	if (Character)
	{
		Character->PlayThrowGrenadeHoldMontage();

	}
}

void UCombatComponent::ThrowGrenade(bool bGrenade)
{
	if (CarriedGrenade == 0) return;
	

	if (CombatState != ECombatState::ECS_Unoccupied || EquippedWeapon == nullptr) return;
	if (Character == nullptr) return;

	CombatState = ECombatState::ECS_ThrowGrenade;

	bCanFire = true;
	
	if (Character)
	{
		ShowGrenadeMesh(false);
		Character->PlayThrowGrenadeMontage();
		
	}

	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();

	}

	if (Character && Character->HasAuthority())
	{
		CarriedGrenade = FMath::Clamp(CarriedGrenade - 1, 0, MaxCarriedGrenade);
		UpdateCarriedGrenade();

	}


}

void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (CombatState != ECombatState::ECS_Unoccupied || EquippedWeapon == nullptr) return;
	if (CarriedGrenade == 0) return;


	CombatState = ECombatState::ECS_ThrowGrenade;

	if (Character)
	{
		ShowGrenadeMesh(true);
		Character->PlayThrowGrenadeMontage();
		
	}

	// 수류탄 카운팅
	CarriedGrenade = FMath::Clamp(CarriedGrenade - 1, 0, MaxCarriedGrenade);
	UpdateCarriedGrenade();

}

void UCombatComponent::ThrowGrenadeHoldFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;
	
}

void UCombatComponent::UpdateCarriedGrenade()
{
	Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedGrenade(CarriedGrenade);

	}
}

// anim 블프에서 사용 액터를 스폰시킬 시간
void UCombatComponent::GrenadeStart()
{
	ShowGrenadeMesh(false);

	if (Character && Character->IsLocallyControlled())
	{
		ServerGrenadeStart(HitTarget);

		//LocalGrenade(HitTarget);
	}
}

void UCombatComponent::ServerGrenadeStart_Implementation(const FVector_NetQuantize& Target)
{
	if (Character && Character->GetGrenadeMesh() && GrenadeClass /*&& /*Character->HasAuthority()*//*test*/)
	{
		// 캐릭터 메시
		FVector StartLocation = Character->GetGrenadeMesh()->GetComponentLocation();
		FVector ToTarget = Target - StartLocation;

		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = Character;  // getowner 는 액터가 소유한 액터의 포인터를 가져온다
		ActorSpawnParameters.Instigator = Character;

		UWorld* world = GetWorld();

		if (world)
		{
			//test code
			if (!Character->HasAuthority())
			{
				// debug
				GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, TEXT("SpawnActor"));

			}
			
			world->SpawnActor<AProjectile>(GrenadeClass, StartLocation, ToTarget.Rotation(), ActorSpawnParameters);
		}
	}


}

//test multi cast
//void UCombatComponent::MultiGrenadeStart_Implementation(const FVector_NetQuantize& Target)
//{
//	// test code
//	//if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
//	//LocalGrenade(Target);
//
//	
//
//
//}

//void UCombatComponent::LocalGrenade(const FVector_NetQuantize& Target)
//{
//	
//
//
//}

bool UCombatComponent::IsGrenadeEmpty()
{
	return CarriedGrenade <= 0;
}

bool UCombatComponent::CanThrowGrenade()
{
	if (EquippedWeapon == nullptr) return false;

	return IsGrenadeEmpty() && bCanThrowGrenade && CombatState == ECombatState::ECS_Unoccupied;

}

// 블루프린트에서 사용
void UCombatComponent::GrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;

}

//// 블루프린트에서 사용
//void UCombatComponent::GrenadeHoldFinished()
//{
//	CombatState = ECombatState::ECS_Unoccupied;
//
//}

// 서버  (서버가 진짜)
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	
	// 다른 무기를 착용하면 다음 무기를 떨어뜨린다
	//WeaponDropped();  // 임시 주석 케릭터가 생성되고 나서 또 equipweapon 함수가 실행되는 모습에 주석처리했음


	// weapon class 대입
	EquippedWeapon = WeaponToEquip;
	// 무기상태 
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

	// 오른손에 무기 attach 함수 
	AttachActorToRightHandSocket(EquippedWeapon); // 리펙토링

	/* &&&&&&&&&&&&&
	무기를 장착 할때  무기 소유자가 캐릭터여야 하며 (Owner 라는 개념은 아주 중요하다)
	플레이어가 제어하고 있는 pawn을 소유하고 있다고 말하지만 무기와 같은 액터에는 정의된 소유자가 없다
	그래서 장비를 장착하자마자 장비를 장착한 캐릭터에게 배정해야 한다  그래서 장착된 무리를 가져갈 수 있고 액터를 가져가는 owner를 호출할 수 있음
	*/
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();

	//=== 리펙토링
	WeaponCarriedAmmo();
	//WeaponPickupSound();
	ReloadWeapon();
	//===

	EquippedWeapon->ShowPickupWidget(false);



	Character->GetCharacterMovement()->bOrientRotationToMovement = false;	// default = false
	Character->bUseControllerRotationYaw = true; // default  = true


}

void UCombatComponent::WeaponCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;

	// EWeaponTypes의 값이 있다면 참
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponTypes()))
	{
		// 참이라면 CarriedAmmo에 EWeaponTypes enum 클래스 가져오기
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponTypes()];
	}
	Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}

}

void UCombatComponent::WeaponPickupSound()
{
	if (Character == nullptr && EquippedWeapon == nullptr && EquippedWeapon->EquipWeaponSound) return;

	// 총기 픽업 사운드
	if (EquippedWeapon->EquipWeaponSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquippedWeapon->EquipWeaponSound, Character->GetActorLocation());

	}
}

void UCombatComponent::ReloadWeapon()
{
	if (EquippedWeapon == nullptr) return;
	// 탄약 0 일때 재장전
	if (EquippedWeapon->IsEmpty())
	{
		// 실질적 탄약 소진 함수
		Reload();
	}
}

void UCombatComponent::WeaponDropped()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
		//EquippedWeapon->SetActorEnableCollision(false); //test
	}

}

// 무기 오른손 attach 함수
void UCombatComponent::AttachActorToRightHandSocket(AActor* ActortoAttach)
{
	// if check 
	if (Character == nullptr || Character->GetMesh() == nullptr || ActortoAttach == nullptr) return;


	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		// 입력값을 받고 무기를 장착할 actor를 연결한다  (골격 메쉬 컴포넌트도 필요)
		HandSocket->AttachActor(ActortoAttach, Character->GetMesh());
	}

}

void UCombatComponent::AttachActorToLeftHandSocket(AActor* ActorToAttach)
{


}

void UCombatComponent::ShowGrenadeMesh(bool visible)
{
	Character->GetGrenadeMesh()->SetVisibility(visible);

}

// 클라 복사
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		// 무기상태 
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);

		// 무기 오른손 소켓 부착 함수
		AttachActorToRightHandSocket(EquippedWeapon);

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;

		// 픽업 사운드
		WeaponPickupSound();
		
		// 서버만 총알 HUD가 안나오는 버그를 고칠 코드
		EquippedWeapon->SetHUDAmmo();

	}
}

void UCombatComponent::InterpFOV(float DeltaTime)
{
	// 우선 줌하기 전  캐릭터가 무기를 장착하고 있는지 확인
	if (EquippedWeapon == nullptr) return;

	if (bAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::InterpCamera(float Delta)
{
	// 우선 줌하기 전  캐릭터가 무기를 장착하고 있는지 확인
	if (EquippedWeapon == nullptr) return;
	if (Character == nullptr) return;

	//FirstCamera = Character->GetFollowCamera()->GetRelativeLocation();
	SecondCamera = Character->GetSecondCamera()->GetRelativeLocation();

	class UCameraComponent* Camera = Character->GetFollowCamera();

	// target (X=130.000610,Y=55.000015,Z=-30.000000

	if (bAiming)
	{
		CurrentCameraLocation = FMath::VInterpTo(CurrentCameraLocation, SecondCamera, Delta, 7.f);
	}
	else
	{
		CurrentCameraLocation = FMath::VInterpTo(CurrentCameraLocation, DefaultCameraLocation, Delta, 3.f);
	}

	if (Character && Character->GetFollowCamera())
	{
		Camera->SetRelativeLocation(CurrentCameraLocation);
	}
}
