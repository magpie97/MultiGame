// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	/*UPROPERTY(EditAnywhere, Category = CharacterMovement)
	float Sprintmodifier;

	virtual float GetMaxSpeed() const override;*/



	// test code
	//UBaseCharacterMovementComponent();

	//bool Safe_bWantsToSprint;

	//class FSaveMove_BaseCharacter : FSavedMove_Character
	//{
	//	typedef FSavedMove_Character Super;

	//	uint8 Saved_bWantsToSprint : 1;

	//	// 동작을 변경하지 않고 복제를 위해 이 이동을 NewMove와 결합할 수 있는 경우 true를 반환합니다.
	//	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;

	//	/** 재사용할 수 있도록 저장된 이동 속성을 지웁니다. */
	//	virtual void Clear() override;

	//	/** 인코딩된 특수 이동 정보(점프, 웅크리기 등)를 포함하는 바이트를 반환합니다. */
	//	virtual uint8 GetCompressedFlags() const override;

	//	/** 예측 수정을 위해 이 저장된 이동(처음 생성된 경우)을 설정하기 위해 호출됩니다. */
	//	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	//	/** ClientUpdatePosition이 예측 수정을 위해 이 SavedMove를 사용하기 전에 호출됨 */
	//	virtual void PrepMoveFor(ACharacter* C) override;

	//};






	
};
