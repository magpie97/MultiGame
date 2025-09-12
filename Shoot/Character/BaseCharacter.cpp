// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "Shoot/ShootComponent/PhysicalAnimation.h"
#include "BaseCharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/ShootComponent/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimMontage.h"
#include "BaseAnimInstance.h"
#include "Shoot/Shoot.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/ShootingGameMode.h"
#include "TimerManager.h"
#include "Shoot/Weapon/WeaponTypes.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Components/TimeLineComponent.h"
#include "Shoot/ShooterPlayerState/ShooterPlayerState.h"
#include "Math/UnrealMathUtility.h"
#include "Shoot/HUD/OverheadWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerState.h"
#include "Components/TextBlock.h"
#include "Shoot/ShootComponent/ServerSideRewindComponent.h"
#include "Shoot/GameState/ShooterGameState.h"





// Sets default values
ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjInit) : 
	Super(ObjInit.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true; // default  true
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;
	SetReplicateMovement(true);	//test 

	// 카메라 암
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());  
	//CameraBoom->SetupAttachment(GetMesh()); // test
	//CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 160.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 2.f;
	CameraBoom->CameraLagMaxDistance = 1.5f;

	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraRotationLagSpeed = 15.f;
	CameraBoom->CameraLagMaxTimeStep = 1.f;

	// 카메라
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	NextCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("NextCamera"));
	NextCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	NextCamera->bUsePawnControlRotation = false;

	// 캐릭터 왼쪽 카메라 testing

	// change camera view location
	//bSecondCamera = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 캐릭터 회전 값
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f);		// in yaw value : 800.f
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 990.f);		// test
	GetCharacterMovement()->JumpZVelocity = 530.f;
	GetCharacterMovement()->AirControl = 0.25f;

	WidgetComponentClass = CreateDefaultSubobject<UWidgetComponent>(TEXT("UserNameWidget"));
	WidgetComponentClass->SetupAttachment(GetCapsuleComponent());

	// 전투 기능을 처리할 컴포넌트   
	// 무기 장착을 담당할 것
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	//Combat->SetIsReplicated(true);  test

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	// 카메라 충돌 처리
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	// net
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadaMesh"));
	GrenadeMesh->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ServerSideRewindComponent = CreateDefaultSubobject<UServerSideRewindComponent>(TEXT("ServerSideRewindComponent"));


	/*
		패킷손실로 인한 게임 랙 해결 방안
		콜리전을 사용하여 패킷이 손실된 거리까지 콜리전 위치를 저장하고 최대한의 이득을 취한다
	*/

	// 이 컴포넌트는 현제 서버에서만 사용하기에 복사 할 필요 없다
	// 머리 부분
	HitBoxHead = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxHead"));
	HitBoxHead->SetupAttachment(GetMesh(), TEXT("head"));
	HitBoxesMap.Add(FName("head"), HitBoxHead);

	// 몸통 부분
	HitBoxPelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxPelvis"));
	HitBoxPelvis->SetupAttachment(GetMesh(), TEXT("pelvis"));
	HitBoxesMap.Add(FName("pelvis"), HitBoxPelvis);

	HitBoxSpine_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxSpine_1"));
	HitBoxSpine_1->SetupAttachment(GetMesh(), TEXT("spine_01"));
	HitBoxesMap.Add(FName("spine_01"), HitBoxSpine_1);

	HitBoxSpine_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxSpine_2"));
	HitBoxSpine_2->SetupAttachment(GetMesh(), TEXT("spine_02"));
	HitBoxesMap.Add(FName("spine_02"), HitBoxSpine_2);

	HitBoxSpine_3 = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxSpine_3"));
	HitBoxSpine_3->SetupAttachment(GetMesh(), TEXT("spine_03"));
	HitBoxesMap.Add(FName("spine_03"), HitBoxSpine_3);

	// 팔 부분
	HitBoxUpperArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxUpperArm_L"));
	HitBoxUpperArm_L->SetupAttachment(GetMesh(), TEXT("upperarm_l"));
	HitBoxesMap.Add(FName("upperarm_l"), HitBoxUpperArm_L);

	HitBoxUpperArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxUpperArm_R"));
	HitBoxUpperArm_R->SetupAttachment(GetMesh(), TEXT("upperarm_r"));
	HitBoxesMap.Add(FName("upperarm_r"), HitBoxUpperArm_R);

	HitBoxLowerArm_L = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxLowerArm_L"));
	HitBoxLowerArm_L->SetupAttachment(GetMesh(), TEXT("lowerarm_l"));
	HitBoxesMap.Add(FName("lowerarm_l"), HitBoxLowerArm_L);

	HitBoxLowerArm_R = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxLowerArm_R"));
	HitBoxLowerArm_R->SetupAttachment(GetMesh(), TEXT("lowerarm_r"));
	HitBoxesMap.Add(FName("lowerarm_r"), HitBoxLowerArm_R);

	// 다리 부분
	HitBoxUpperLag_L = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxUpperLag_L"));
	HitBoxUpperLag_L->SetupAttachment(GetMesh(), TEXT("thigh_l"));
	HitBoxesMap.Add(FName("thigh_l"), HitBoxUpperLag_L);

	HitBoxUpperLag_R = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxUpperLag_R"));
	HitBoxUpperLag_R->SetupAttachment(GetMesh(), TEXT("thigh_r"));
	HitBoxesMap.Add(FName("thigh_r"), HitBoxUpperLag_R);

	HitBoxLowerLag_L = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBoxLowerLag_L"));
	HitBoxLowerLag_L->SetupAttachment(GetMesh(), TEXT("calf_l"));
	HitBoxesMap.Add(FName("calf_l"), HitBoxLowerLag_L);

	HitBoxLowerLag_R = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBexLowerLag_R"));
	HitBoxLowerLag_R->SetupAttachment(GetMesh(), TEXT("calf_r"));
	HitBoxesMap.Add(FName("calf_r"), HitBoxLowerLag_R);

	// physical animation component
	//PhysicalAnimationComponent = CreateDefaultSubobject<UPhysicalAnimationComponent>(TEXT("PhysicalAnimationComponent"));

	// 커스텀 콜리전 적용 및 콜리전 비활성화
	for (auto Box : HitBoxesMap)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	



	
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 게임 시작시 기본 장착될 무기 설정
	AttachDefaultWeapon();

	UpdateHUDWeaponAmmo();

	UpdateHUDHealth();

	/*if (PhysicalAnimationComponent)
	{
		PhysicalAnimationComponent->SetSkeletalMeshComponent(GetMesh());
		
		GetMesh()->SetAllBodiesBelowSimulatePhysics(FName("pelvis"), true, false);

	}*/
	
	// 서버확인
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::ApplyDamage);

	}

	if (GrenadeMesh)
	{
		GrenadeMesh->SetVisibility(false);

	}
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Axis Mapping
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABaseCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ABaseCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ABaseCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ABaseCharacter::LookUp);

	// Actions Mapping
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ABaseCharacter::Jump);
	//PlayerInputComponent->BindAction(FName("Jump"), IE_Released, this, &ABaseCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Sprint"), IE_Pressed, this, &ABaseCharacter::SprintButtonPressed);
	PlayerInputComponent->BindAction(FName("Sprint"), IE_Released, this, &ABaseCharacter::SprintButtonReleased);
	PlayerInputComponent->BindAction(FName("Fire"), IE_Pressed, this, &ABaseCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction(FName("Fire"), IE_Released, this, &ABaseCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction(FName("Reload"), IE_Pressed, this, &ABaseCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction(FName("ThrowGrenade"), IE_Pressed, this, &ABaseCharacter::GrenadeButtonPressed);
	PlayerInputComponent->BindAction(FName("ThrowGrenade"), IE_Released, this, &ABaseCharacter::GrenadeButtonReleased);
	
	// testsprint
	/*PlayerInputComponent->BindAction(FName("Sprint"), IE_Pressed, this, &ABaseCharacter::OnStartSprint);
	PlayerInputComponent->BindAction(FName("Sprint"), IE_Released, this, &ABaseCharacter::OnStopSprint);*/

	PlayerInputComponent->BindAction(FName("Crouch"), IE_Pressed, this, &ABaseCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction(FName("Aim"), IE_Pressed, this, &ABaseCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction(FName("Aim"), IE_Released, this, &ABaseCharacter::AimButtonReleased);

	// Interaction Mapping
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ABaseCharacter::EquipButtonPressed);


}

void ABaseCharacter::MoveForward(float Value)
{
	//IsMovingForward = Value > 0.f;

	// Controller check
	if (Controller != nullptr && Value != 0)
	{
		// actor forward vector cal
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);

	}
}

void ABaseCharacter::MoveRight(float Value)
{
	// Controller check
	if (Controller != nullptr && Value != 0)
	{
		// actor forward vector cal
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void ABaseCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
	
}

void ABaseCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

// 재장전
void ABaseCharacter::ReloadButtonPressed()
{
	if (Combat)
	{
		Combat->Reload();
	}
}

void ABaseCharacter::InfoButtonPressed()
{
}

void ABaseCharacter::Info()
{
}

void ABaseCharacter::PlayPhysicalAnimation()
{
	//todo : 피직컬 애니메이션 실행

	//if (PhysicalAnimation)
	//{
	//	//PhysicalAnimation->


	//}

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseCharacter, OverlappingWeapon, COND_OwnerOnly);
	//DOREPLIFETIME_CONDITION(ABaseCharacter, bWantsToRun, COND_SkipOwner);
	//DOREPLIFETIME(ABaseCharacter, bDead);
	DOREPLIFETIME(ABaseCharacter, bDisableGamePlay);

	DOREPLIFETIME(ABaseCharacter, CurrentHealth);
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		// 포인터 초기화
		Combat->Character = this;
	}

	if (ServerSideRewindComponent)
	{
		// 포인터 초기화
		ServerSideRewindComponent->Character = this;
		if (Controller)
		{
			ServerSideRewindComponent->PlayerController = Cast<AShooterPlayerController>(GetController());
		}
	}
	
	
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//deltatime = DeltaTime; test
	PollInit();

	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		// 매 프레임 마다 실행 시켜야 한다
		AimOffset(DeltaTime);

	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)		// defult value 0.25f
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}

	// 캐릭터가 공중에 있다면 rotator 값을 0으로 
	GetCharacterMovement()->IsFalling() == true ? GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f) : GetCharacterMovement()->RotationRate = FRotator(0.f, 800.f, 0.f);

}

void ABaseCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(false);
		}
	}
	
	OverlappingWeapon = Weapon;

	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void ABaseCharacter::EquipButtonPressed()
{
	// UCombatComponent 클래스 변수
	if (Combat)
	{
		if (HasAuthority()) /// 권한 체크 HasAuthority (이렇게 하면 서버만 무기를 호출 할 수 있다)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else
		{
			ServerEquipButtonPressed();
		}
	}
}

void ABaseCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}
}

void ABaseCharacter::Jump()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else 
	{
		Super::Jump();

	}
}

void ABaseCharacter::AttachDefaultWeapon()
{
	GameMode =  GameMode == nullptr ? Cast<AShootingGameMode>(UGameplayStatics::GetGameMode(this)) : GameMode;
	UWorld* World = GetWorld();

	if (GameMode && World && !bDead && DefaultWeaponClass)
	{
		AWeapon* StartDefaultWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);

		StartDefaultWeapon->bDestroyDefaultWeapon = true;

		if (Combat)
		{
			Combat->EquipWeapon(StartDefaultWeapon);
		}
	}
}

//void ABaseCharacter::CameraSwitch()
//{
//	// 현재 사용중인 코드
//	if (bSecondCamera)
//	{
//		FollowCamera->SetRelativeLocation(FVector(0.f, -75.f, 0.f));
//		FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
//		bSecondCamera = false;
//
//	}
//	else
//	{
//		FollowCamera->SetRelativeLocation(FVector(0.f, 75.f, 0.f));
//		FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);
//		bSecondCamera = true;
//	}
//
//}

void ABaseCharacter::FireButtonPressed()
{
	if (Combat)
	{
		Combat->FireButtonPressed(true);
	}
}

void ABaseCharacter::FireButtonReleased()
{
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void ABaseCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, TEXT("GrenadePressed"));
		Combat->ThrowGrenadeHold(true);

	}
}

void ABaseCharacter::GrenadeButtonReleased()
{
	if (Combat)
	{
		//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, TEXT("GrenadeReleased"));
		Combat->ThrowGrenade(true);
	}
}

void ABaseCharacter::SimProxiesTurn()
{
	// null check 매번 중요하다  체크만 잘해도 작업속도가 빨라진다
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	// 먼저 처리할 기능은 루트 뼈대를 돌리는것
	// 시뮬레이션 프록시가 아닌 경우에 서버나 클라이언트나 로컬로 작동된다
	bRotateRootBone = false;

	// turning 할때 캐릭터가 미끄러지듯 움직이는 모습이 보여졌다 이를 수정하기 위해 속력을 리턴하는 함수인 Calculatespeed 함수를 speed 변수에 초기화  speed가 0보다 크다면 notturnning을 실행시켜 미끄러지는 모습을 지웠다
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning; 
		return;
	}

	// 현재 회전값을 마지막 회전값 변수에 초기화
	ProxyRotationLastFrame = ProxyRotationCurrentFrame;
	// 액터의 회전값을 가져오는 함수에 현재 회전값을 초기화
	ProxyRotationCurrentFrame = GetActorRotation();
	// 첫 번째 파라미터 - 두 번째 파라미터 의 값을 리턴
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotationCurrentFrame, ProxyRotationLastFrame).Yaw;

	// abs 해당 변수의 절대값을 리턴한다
	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if(ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}

	// 기본 상태
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;

}

void ABaseCharacter::CrouchButtonPressed()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABaseCharacter::AimButtonPressed()
{
	//Combat->bAiming = false;
	if (Combat)
	{
		// 이렇게 구현하면 서버만 aiming을 하는 애니메이션 실행됨 따라서 combat 컴포넌트 클래스에서 만든 set 함수를 실행
		// Combat->bAiming = true;
		Combat->SetAiming(true);

	}
}

void ABaseCharacter::AimButtonReleased()
{
	// // 위와 같은 이유로 변경해준다
	if (Combat)
	{
		Combat->SetAiming(false);

	}
}

ECombatState ABaseCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

FVector ABaseCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();

	return Combat->HitTarget;
}

// FABKIK 을 위한 함수 
AWeapon* ABaseCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;

	return Combat->EquippedWeapon;
	
}

void ABaseCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

bool ABaseCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool ABaseCharacter::IsAiming()
{

	// 액터 컴포넌트 클래스에 변수값으로 체크
	return (Combat && Combat->bAiming);
}

float ABaseCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ABaseCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir) // 점프하지 않고 가만히 서 있다
	{
		// 스무스 움직임을 처리한 변수
		bRotateRootBone = true;

		// 현재 바라보고 있는 카메라의 회전값을 초기화
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		// 회전이 시작된 회전값과 움직이고 나서 멈춘 회전값을 뺀 나머지 값을 리턴하는 함수  A - B  return delta (rotator 타입 리턴)
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}

		// true라면 yaw는 playercontroller에 의해 제어되는경우에 controller의 controlrotation yaw와 일치하도록 업데이트 됀다
		bUseControllerRotationYaw = true; // true

		// turning in place
		TurnInPlace(DeltaTime);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Yaw : %f"), AO_Yaw));
	}
	if (Speed > 0.f || bIsInAir)
	{
		// 스무스 움직임을 처리한 변수
		bRotateRootBone = false;

		// 카메라가 바라보는 회전값을 초기화
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;

		TurningInPlace = ETurningInPlace::ETIP_NotTurning;

	}

	CalculateAO_Pitch();

}

void ABaseCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;

	// client and server aimoffset 처리 및 버그 수정
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		/*
		이 implimentation section에선 server에서 client의 애니메이션 버그를 수정하는 section이다
		허나 아래로 내려다 봤을때 270도에서 360도 사이 값으로 바뀌어
		client에선 문제없이 보이지만 server에서 보면 외로 치켜 세워 보는 애니메이션이 보여진다


		*/
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);

		// [Input:Range] inclusive로 고정된 주어진 값에 대해 [Output:Range] Inclusive에서 해당 백분율을 반환한다
		// 쉽게 말하면 InRange의 x, y 값을  OutRange의 값으로 바꾸고 마지막 매개변수인 AO_Pitch 변수값을 변경한다
		// (정면에서 위로 볼때 Pitch 값은 0 ~ 90   아래에서 정면을 볼때 Pitch 값은 -270 ~ -360 이걸 aimoffset의 값으로 생각해 변경한다
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void ABaseCharacter::TurnInPlace(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("AO_Yaw : %f"), AO_Yaw);
	
	// log로 표현된 값은 우측으로 는 + 이다
	if (AO_Yaw > 55.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -55.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 2.f);	// 2
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f) // default 25.f
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ABaseCharacter::OnRep_Health()
{
	UpdateHUDHealth();

	PlayHitReactMontage();

}

void ABaseCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABaseCharacter::PlayDeadMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeadMontage)		// bDead 는 없었던것
	{
		AnimInstance->Montage_Play(DeadMontage);

	}
}

void ABaseCharacter::OnRep_ReplicatedMovement()
{
	// 먼저 처리할 기능은 복제된 캐릭터의 움직임에 대해  Super 키워드를 사용한다
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();

	TimeSinceLastMovementReplication = 0.f;
}

void ABaseCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr && Combat->EquippedWeapon == nullptr) return;			// test default || 

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage && !AnimInstance->IsAnyMontagePlaying())
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABaseCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		// 현재 구현된 무기 종류는 라이플 밖에 없으니 하나만 쓴다
		switch (Combat->EquippedWeapon->GetWeaponTypes())
		{
		case EWeaponTypes::EWT_Pistol:
			SectionName = "Pistol";
			break;

		case EWeaponTypes::EWT_Rifle:
			SectionName = "Rifle";
			break;

		case EWeaponTypes::EWT_Grenade:
			SectionName = "Rifle";
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void ABaseCharacter::PlayThrowGrenadeHoldMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeHoldMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeHoldMontage);

	}
}

void ABaseCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
		
	}
}

void ABaseCharacter::SprintButtonPressed()
{
	if (Combat)
	{
		Combat->SetSprint(true);
	}
}

void ABaseCharacter::SprintButtonReleased()
{
	if (Combat)
	{
		Combat->SetSprint(false);
	}
}

bool ABaseCharacter::IsSprint()
{
	return (Combat && Combat->bSprint);
}

void ABaseCharacter::Dead()
{
	
	if (Combat && Combat->EquippedWeapon)
	{
		if (Combat->EquippedWeapon->bDestroyDefaultWeapon)
		{
			Combat->EquippedWeapon->Destroy();
			Combat->EquippedWeapon->GetWeaponMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Combat->EquippedWeapon->GetWeaponMesh()->SetVisibility(true);
		}
		else
		{
			Combat->EquippedWeapon->Dropped();
		}
	}

	ServerDead();

}

void ABaseCharacter::ServerDead_Implementation()
{
	MulticastDead();

}

void ABaseCharacter::MulticastDead_Implementation()
{
 	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDWeaponAmmo(0);
	}

	bDead = true;
	
	// 피직스 시뮬때문에 사용하지는 않음
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(AnimInstance)
	{
		PlayDeadMontage();
	}

	if (Combat)
	{
		Combat->FireButtonPressed(false);

	}

	if (ShooterPlayerController)
	{
		DisableInput(ShooterPlayerController);
		
		//GetEquippedWeapon()->SetOwner(nullptr); // test
		//BaseCharacter->SetOwner(nullptr); // test
		//if (GetOwner() == nullptr)
		//{
		//	GetEquippedWeapon()->SetOwner(nullptr); // 죽어도 총을 쏠 수 있어서 혹시 오너 null test

		//}
	}

	if (DeadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeadSound, this->GetActorLocation());

	}

	// 피직스
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//GetCapsuleComponent()->Deactivate();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetCharacterMovement()->DisableMovement();// can't input w a s d Keys

	//GetCharacterMovement()->StopMovementImmediately();	// can't input mouse rotation

	// 캐리터가 죽고 다시 리셋
	this->GetWorldTimerManager().SetTimer(DeadTimer, this, &ABaseCharacter::DeadTimerFinished, DeadDelay); //  임시 주석 클라에 있던것
	
}

void ABaseCharacter::DeadTimerFinished()
{
	AShootingGameMode* ShootingGameMode = GetWorld()->GetAuthGameMode<AShootingGameMode>();

	this->GetWorldTimerManager().ClearTimer(DeadTimer);

	if (ShootingGameMode)
	{
		ShootingGameMode->PlayerRespawn(this, Controller);

	}
}

void ABaseCharacter::ApplyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	// 게임 실행중 게임모드의 포인터를 가져올때 GetAuthGameMode 함수를 사용
// 멀티 게임에서 게임모드는 게임을 관리하는 관리자 역활하여 게임에서 중요한 데이터를 인증하는 권한을 가진다
	GameMode = GameMode == nullptr ? GetWorld()->GetAuthGameMode<AShootingGameMode>() : GameMode;
	if (bDead || GameMode == nullptr) return;

	// 현재 health 가 0 밑으로 내려가지 않도록 클램핑 한다
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	UpdateHUDHealth();

	PlayHitReactMontage();

	// 게임모드  관련 코드
	// 캐릭터가 죽으면 다시 리스폰 시키거나 게임 전체 스코어를 카운팅, 점수를 처리하기위해 작성함
	if (CurrentHealth == 0.f)
	{
		// check
		if (GameMode)
		{
			ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
			AShooterPlayerController* AttackerPlayerController = Cast<AShooterPlayerController>(InstigatorController);

			GameMode->PlayerDead(this, ShooterPlayerController, AttackerPlayerController);
		}
	}
}

void ABaseCharacter::UpdateHUDHealth()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDHealth(CurrentHealth, MaxHealth);

	}
}

void ABaseCharacter::UpdateHUDWeaponAmmo()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(GetController()) : ShooterPlayerController;

	// controller 와 Equippedwepaon 이 null 값임
	if (ShooterPlayerController && Combat && Combat->EquippedWeapon)
	{
		ShooterPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		ShooterPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
		//ShooterPlayerController->SetHUDCarriedGrenade(Combat->CarriedGrenade);  //test


	}
}

void ABaseCharacter::PollInit()
{
	if (ShooterPlayerState == nullptr)
	{
		ShooterPlayerState = GetPlayerState<AShooterPlayerState>();

		if (ShooterPlayerState)
		{
			ShooterPlayerState->AddToScore(0.f);
			ShooterPlayerState->AddToDeathScore(0);
			
		}
	}

	if (ShooterPlayerController == nullptr)
	{
		ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
		if(ShooterPlayerController)
		{
			AttachDefaultWeapon(); // test
			UpdateHUDWeaponAmmo();
			
			
		}
	}
}

