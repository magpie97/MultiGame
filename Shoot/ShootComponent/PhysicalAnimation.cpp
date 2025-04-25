// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicalAnimation.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "PhysicsEngine/PhysicalAnimationComponent.h"

void UPhysicalAnimation::BeginPlay()
{
	Super::BeginPlay();

	BaseCharacter = BaseCharacter == nullptr ? Cast<ABaseCharacter>(GetOwner()) : BaseCharacter;

	if (PhysicalAnimation == nullptr) return;

	if (BaseCharacter)
	{
		USkeletalMeshComponent* mesh = BaseCharacter->GetMesh();

		PhysicalAnimation->SetSkeletalMeshComponent(mesh);


	}
}