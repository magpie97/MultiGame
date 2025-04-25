// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"
#include "PhysicalAnimation.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UPhysicalAnimation : public UPhysicalAnimationComponent
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY()
	class ABaseCharacter* BaseCharacter;

	UPROPERTY()
	class UPhysicalAnimation* PhysicalAnimation;




public:




public:

	//FORCEINLINE GetPhysicalAnimation() const { return PhysicalAnimation; }
	
};
