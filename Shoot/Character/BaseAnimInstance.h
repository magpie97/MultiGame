// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/ShootType/ShootType.h"
#include "BaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	// 아래 함수는 각 단계에 대한 기본 재정의입니다.
	// 기본 초기화 재정의 지점
	virtual void NativeInitializeAnimation() override;

	// 기본 업데이트 재정의 지점입니다. 일반적으로 이 단계에서 단순히 데이터를 수집하고
	// NativeUpdateAnimation에서 수행할 작업의 대부분을 위해.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	class ABaseCharacter* BaseCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	//UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	//float Sprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	float MovementOffsetYaw;

	// 현재 장착된 무기가 있는지 체크할 방법
	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped;

	class AWeapon* EquippedWeapon;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bAiming;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bSprint;

	// BS에 있는 캐릭터 애니메이션 값의 이름
	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	float YawOffset;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	float LastYawOffset;

	// BS에 있는 기울기 값
	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Lean;
	
	// 캐릭터 회전 값
	FRotator CharacterRotation;

	// 캐릭터 마지막 회전 값
	FRotator CharacterRotationLastFrame;

	// interp 함수를 사용하여 부드러운 움직임을 구현 하려 했지만 -180도 에서 180도 까지 움직이다보니 버벅거리는 애니메이션이 보여진다
	// 따라서 제일 짧은 회전값을 저장하려는 변수를 선언
	FRotator DeltaRotation;

	// aim offset 세팅에 필요한 변수
	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	float AO_Yaw;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	float AO_Pitch;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	FTransform LeftHandTransform;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	ETurningInPlace TurningInPlace;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bLocallyControlled;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bRotateRootBone;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bDead;

	UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bReloading;

	//UPROPERTY(BlueprintReadWrite, Category = Character, meta = (AllowPrivateAccess = "true"))
	//bool bGrenadeHold;





};
