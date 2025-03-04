// Fill out your copyright notice in the Description page of Project Settings.


#include "PktLagComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

UPktLagComponent::UPktLagComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;

	
}

void UPktLagComponent::BeginPlay()
{
	Super::BeginPlay();

	
	FPktLagStruct PktLagStruct;

	// PktLag ������Ʈ�� ������ ����ü 
	PktLagSaveLocation(PktLagStruct);

	// ����� PktLag ����ü debug
	PktLagDebugCapsule(PktLagStruct, FColor::Red);

	
}

void UPktLagComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UPktLagComponent::PktLagSaveLocation(FPktLagStruct& PktLagStruct)
{
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;
	//PlayerController = PlayerController == nullptr ? Cast<AShooterPlayerController>(Controller)

	PktLagStruct.PktLagTime = GetWorld()->GetTimeSeconds();

	// ���ε� collision component�� ��ġ, ȸ��, ����, ���� ������ ���� ������ �� �����Դ��� debug�� �ۼ�
	for (auto& PktLagCapsule : Character->PktLagMappingComponent)
	{
		// Map �����̳ʸ� ����ϱ⿡ key, value�� �����Ͽ� ��� ����
		FPktLagCapsuleTransform PktLagCT;
		PktLagCT.PktLagCapsuleComponentLocation = PktLagCapsule.Value->GetComponentLocation();			// ��ġ
		PktLagCT.PktLagCapsuleComponentRotator = PktLagCapsule.Value->GetComponentRotation();			// ȸ��
		PktLagCT.PktLagCapsuleComponentRadius = PktLagCapsule.Value->GetScaledCapsuleRadius();			// ������
		PktLagCT.PktLagCapsuleComponentHalfHeight = PktLagCapsule.Value->GetScaledCapsuleHalfHeight();	// ���� ����

		// ���������� ����� �����̳ʿ� ����
		PktLagStruct.PktLagCapsuleMap.Add(PktLagCapsule.Key, PktLagCT);

	}
}

void UPktLagComponent::PktLagDebugCapsule(FPktLagStruct& PktLagStruct, FColor Color)
{
	for (auto& DebugCapsule : PktLagStruct.PktLagCapsuleMap)
	{
		DrawDebugCapsule(GetWorld(), DebugCapsule.Value.PktLagCapsuleComponentLocation, DebugCapsule.Value.PktLagCapsuleComponentHalfHeight, DebugCapsule.Value.PktLagCapsuleComponentRadius, FQuat(DebugCapsule.Value.PktLagCapsuleComponentRotator), Color, true);

		//DrawDebugCapsule(GetWorld(), DebugCapsule.Value.PktLagCapsuleComponentLocation, DebugCapsule.Value.PktLagCapsuleComponentScale., 0.2f, FQuat(DebugCapsule.Value.PktLagCapsuleComponentRotator), Color, true);

		//GEngine->AddOnScreenDebugMessage(3, 15.f, FColor::Orange, TEXT("Capsule Scale : %s"), *DebugCapsule.Value.PktLagCapsuleComponentScale.ToString());
		//DrawDebugCapsule(GetWorld(), DebugCapsule.Value.PktLagCapsuleComponentLocation, DebugCapsule.Value.PktLagCapsuleComponentScale.get)
		//Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

	}
}