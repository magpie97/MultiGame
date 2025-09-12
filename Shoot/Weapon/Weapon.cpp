// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/HUD.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/Weapon/WeaponCameraShake.h"
#include "Camera/PlayerCameraManager.h"


// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(WeaponMesh);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(WeaponMesh);

}


bool AWeapon::IsEmpty()
{

	return Ammo <= 0;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// 
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}


	// 크래킹 test
	/*if (!HasAuthority())
	{
		FireDelay = 0.005f;


	}*/

}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 여기서 복제하고 싶은 변수를 등록할 수 있다
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bUseServerSideRewind, COND_OwnerOnly);

}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (BaseCharacter)
	{
		//GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Red, FString(TEXT("Overlapping weapon collision")));

		// 위젯 표시 (E)
		BaseCharacter->SetOverlappingWeapon(this);
	}

}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (BaseCharacter)
	{
		//GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Red, FString(TEXT("Out the weapon collision")));

		BaseCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetEnableGravity(false);
		break;

	case EWeaponState::EWS_Dropped:
		
		if (HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}

		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		break;
	}

}


void AWeapon::SpendRound()
{
	// 총을 쏠때마다 총알 개수를 업데이트 하기 위한 함수


	// 0 이하로 안떨어지게 하기
	Ammo = FMath::Clamp(Ammo, 0, MagCapacity);
	--Ammo;

	SetHUDAmmo();

	// 총알 한발은 문제 없지만 두번째때 총알 쏠 경우 
	// 서버인지 확인 하는 함수   이 함수는 싱글에선 안쓰임  멀티 에서 서버로 구동되는 컴퓨터에서만 작동되는 함수이다  
	// 절대 까먹지 말자 이거 은근 자주 사용하고 디버깅할때 유용하다  서버면 true를 리턴하는 함수이다    
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;


	}
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	// 이 함수는 총을 발사 한 후 처리하는 hud가 네트워크 지연으로 인한 업데이트가 늦게 이루어지는 상황에서 해결할 코드이다. 
	// 100ms 이상이라면 확연히 차이가 보여진다.

	if (HasAuthority()) return;

	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();


}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);  

	SetHUDAmmo();

	ClientAddAmmo(AmmoToAdd);
}

void AWeapon::ClientAddAmmo_Implementation(int32 AddToAmmo)
{
	if (HasAuthority()) return;

	Ammo = FMath::Clamp(Ammo + AddToAmmo, 0, MagCapacity);

	SetHUDAmmo();


}

bool AWeapon::IsFullAmmo()
{
	
	return Ammo == MagCapacity;
	
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();

	if (GetOwner() == nullptr)		// owner가 private 에 선언되어서 get함수 사용했음
	{
		// 유저가 무기를 소기할때 owner가 캐릭터로 변경  무기를 변경 할때 
		// 이미 소지중인 무기의 owenr를 null로 초기화하여 다른 유저가 들 수 있도록 한다
		OwnerBaseCharacter = nullptr;
		OwnerShooterPlayerController = nullptr;
	}
	else
	{
		OwnerBaseCharacter = OwnerBaseCharacter == nullptr ? Cast<ABaseCharacter>(GetOwner()) : OwnerBaseCharacter;
		if (OwnerBaseCharacter && OwnerBaseCharacter->GetEquippedWeapon() && OwnerBaseCharacter->GetEquippedWeapon() == this)
		{
			SetHUDAmmo();
		}
		
	}
}

void AWeapon::SetHUDAmmo()
{
	OwnerBaseCharacter = OwnerBaseCharacter == nullptr ? Cast<ABaseCharacter>(GetOwner()) : OwnerBaseCharacter;
	if (OwnerBaseCharacter)
	{
		OwnerShooterPlayerController = OwnerShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(OwnerBaseCharacter->GetController()) : OwnerShooterPlayerController;
		if (OwnerShooterPlayerController)
		{
			OwnerShooterPlayerController->SetHUDWeaponAmmo(Ammo);

		}
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimations)
	{
		WeaponMesh->PlayAnimation(FireAnimations, false);

		
	}

	// 지연시간에 대응할 조치  
	SpendRound();  // 우선 임시 주석 위 코드 는  test
	

}

void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(false);
		break;

	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		break;
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Dropped()
{
	// weapon state를 drop으로 set해주고
	SetWeaponState(EWeaponState::EWS_Dropped);

	// 컴포넌트를 캐릭터의 skeletal mesh name에 detach 하는 함수 실행 
	FDetachmentTransformRules KeepWorldTransform(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(KeepWorldTransform);
	SetOwner(nullptr);		// 캐릭터가 죽고 drop 하는 weapon는 더이상 캐릭터의 주인이 아니기에 null 값을 set해준다  (무기 얻을때는 캐릭터의 오너로 변경해줬다는걸 잊지말기)

	OwnerBaseCharacter = nullptr;
	OwnerShooterPlayerController = nullptr;



}