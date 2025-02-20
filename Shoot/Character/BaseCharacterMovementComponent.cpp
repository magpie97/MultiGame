// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

 //test code
//UBaseCharacterMovementComponent::UBaseCharacterMovementComponent()
//{
//
//}
//
//bool UBaseCharacterMovementComponent::FSaveMove_BaseCharacter::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
//{
//	FSaveMove_BaseCharacter* NewBaseCharacterMove = static_cast<FSaveMove_BaseCharacter*>(NewMove.Get());
//
//	if (Saved_bWantsToSprint != NewBaseCharacterMove->Saved_bWantsToSprint)
//	{
//		return false;
//	}
//
//	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
//}
//
//void UBaseCharacterMovementComponent::FSaveMove_BaseCharacter::Clear()
//{
//	FSavedMove_Character::Clear();
//
//	Saved_bWantsToSprint = 0;
//}
//
//uint8 UBaseCharacterMovementComponent::FSaveMove_BaseCharacter::GetCompressedFlags() const
//{
//	uint8 Result = Super::GetCompressedFlags();
//
//	if (Saved_bWantsToSprint)
//	{
//		Result = FLAG_Custom_0;
//	}
//
//	return uint8();
//}
//
//void UBaseCharacterMovementComponent::FSaveMove_BaseCharacter::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
//{
//}
//
//void UBaseCharacterMovementComponent::FSaveMove_BaseCharacter::PrepMoveFor(ACharacter* C)
//{
//}

//float UBaseCharacterMovementComponent::GetMaxSpeed() const
//{
//	const float MaxSpeed = Super::GetMaxSpeed();
//	const ABaseCharacter* Character = Cast<ABaseCharacter>(GetPawnOwner());
//
//	return Character && Character->;
//}
