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

	//	// ������ �������� �ʰ� ������ ���� �� �̵��� NewMove�� ������ �� �ִ� ��� true�� ��ȯ�մϴ�.
	//	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;

	//	/** ������ �� �ֵ��� ����� �̵� �Ӽ��� ����ϴ�. */
	//	virtual void Clear() override;

	//	/** ���ڵ��� Ư�� �̵� ����(����, ��ũ���� ��)�� �����ϴ� ����Ʈ�� ��ȯ�մϴ�. */
	//	virtual uint8 GetCompressedFlags() const override;

	//	/** ���� ������ ���� �� ����� �̵�(ó�� ������ ���)�� �����ϱ� ���� ȣ��˴ϴ�. */
	//	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character& ClientData) override;

	//	/** ClientUpdatePosition�� ���� ������ ���� �� SavedMove�� ����ϱ� ���� ȣ��� */
	//	virtual void PrepMoveFor(ACharacter* C) override;

	//};






	
};
