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

	// PktLag 컴포넌트를 저장할 구조체 
	PktLagSaveLocation(PktLagStruct);

	// 저장된 PktLag 구조체 debug
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

	// 매핑된 collision component를 위치, 회전, 지름, 절반 높이의 값을 가져와 잘 가져왔는지 debug를 작성
	for (auto& PktLagCapsule : Character->PktLagMappingComponent)
	{
		// Map 컨테이너를 사용하기에 key, value로 지정하여 요소 참조
		FPktLagCapsuleTransform PktLagCT;
		PktLagCT.PktLagCapsuleComponentLocation = PktLagCapsule.Value->GetComponentLocation();			// 위치
		PktLagCT.PktLagCapsuleComponentRotator = PktLagCapsule.Value->GetComponentRotation();			// 회전
		PktLagCT.PktLagCapsuleComponentRadius = PktLagCapsule.Value->GetScaledCapsuleRadius();			// 반지름
		PktLagCT.PktLagCapsuleComponentHalfHeight = PktLagCapsule.Value->GetScaledCapsuleHalfHeight();	// 절반 높이

		// 실질적으로 사용할 컨테이너에 저장
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