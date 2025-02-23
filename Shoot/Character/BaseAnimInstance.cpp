// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnimInstance.h"
#include "BaseCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shoot/ShootComponent/CombatComponent.h"

#include "Engine/Engine.h"
#include "BaseCharacterMovementComponent.h"
#include "Shoot/Weapon/Weapon.h"

#include "DrawDebugHelpers.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());


}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	// null 체크
	if (BaseCharacter == nullptr)
	{
		BaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
	}
	if (BaseCharacter == nullptr) return;

	// 캐릭터의 속도를 가져오기
	FVector Velocity = BaseCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	// 공중에 있는지 판단
	bIsInAir = BaseCharacter->GetCharacterMovement()->IsFalling();

	// 가속하고 있는지 판단
	bIsAccelerating = BaseCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;

	bWeaponEquipped = BaseCharacter->IsWeaponEquipped();

	bIsCrouched = BaseCharacter->bIsCrouched;
	bAiming = BaseCharacter->IsAiming();
	bSprint = BaseCharacter->IsSprint();
	EquippedWeapon = BaseCharacter->GetEquippedWeapon();
	TurningInPlace = BaseCharacter->GetTurningInPlace();
	bRotateRootBone = BaseCharacter->GetRotateRootBone();	// foceinline shouldRotateRootBone 
	bDead = BaseCharacter->GetDead();
	//bGrenadeHold = BaseCharacter->GetGrenadeHold();
	


	// BS 애니메이션 사용에 필요한 함수  카메라의 바라 보는 방향을 구할 수 있다
	FRotator AimRotation = BaseCharacter->GetBaseAimRotation();

	// 이 함수는 입력 파라미터 벡터를 X축으로 하여 새로운 기저를 구축하고, 그 기저로의 회전 행렬을 리턴한다. 
	// 캐릭터가 바라보는 방향 즉, x 축 방향의 값을 가져올 수 있다
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(BaseCharacter->GetVelocity());

	/*
	* rotator 첫번째 매개변수와 두번째 매개변수 값을 뺀 나머지 값을 리턴하는 함수이다  
	interp 함수를 사용하여 부드러운 움직임을 구현 하려 했지만 -180도 에서 180도 까지 움직이다보니 버벅거리는 애니메이션이 보여진다
	따라서 제일 짧은 회전값을 저장하려는 변수를 선언
	*/
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 10.f);
	YawOffset = DeltaRotation.Yaw;

	/*
	* 캐릭터의 Lean 기능 구현
	* 캐릭터의 회전값, 최신 회전값을 저장할 변수 필요
	* 최신 회전값을 저장할 변수에 getactorrotation 함수를 사용
	* normalizeddeltarotator 라는 함수는 :: 두 로테이션 사이의 차이를 로테이션으로 반환, A를 기준으로 B는 얼마나 더 회전했는지를 반환받고 싶을 때 사용하면 됩니다.
	* 반환한 rotation 값을 delta 변수에 초기화 
	* a - b 로테이션 차이 값을 저장한 delta yaw 값과 틱 시간인 deltaseconds 값을 나눈 Target 변수 초기화
	* 자연스러운 Lean 값을 변경하기 위해 FInterpTo 함수 사용 
	* 값을 변경할 Lean값, Target, tick, 보간 값
	* Clamp 함수로 -180 ~ 180 인 Lean 값을 90도로 변경
	* 
	*/
	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = BaseCharacter->GetActorRotation();
	FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	float Target = Delta.Yaw / DeltaSeconds;
	float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 10.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);
	

	// 캐릭터의 stop 애니메이션을 위한 기능
	if (BaseCharacter->GetVelocity().Size() > 0.f)
	{
		LastYawOffset = YawOffset;
	}

	AO_Yaw = BaseCharacter->GetAO_Yaw();
	AO_Pitch = BaseCharacter->GetAO_Pitch();
	
	// 재장전 중 수류탄을 던질수 없는 상태를 비어있는 상태로 변경하면 장전중에도 던질 수 있다    만약 안되면 다시 돌리기
	//bReloading = BaseCharacter->GetCombatState() != ECombatState::ECS_Reloading;    
	bReloading = BaseCharacter->GetCombatState() != ECombatState::ECS_Unoccupied;

	}