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

	
	//FPktLagStruct PktLagStruct;

	//// PktLag 컴포넌트를 저장할 구조체 
	//PktLagSaveLocation(PktLagStruct);

	//// 저장된 PktLag 구조체 debug
	//PktLagDebugCapsule(PktLagStruct, FColor::Red);

	
}

void UPktLagComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 0번째 키 위치에 (head) 접근하여 확인 
	if (PktLagLocationHistoryLinkedList.Num() <= 1)
	{
		FPktLagCapsuleTransformMap PktLagCapsuleTransformMap;

		// 캡슐 컴포넌트 transform 값을 저장 후
		PktLagSaveLocation(PktLagCapsuleTransformMap);

		// 맨 앞 (head)부분에 transform 값을 저장
		PktLagLocationHistoryLinkedList.AddHead(PktLagCapsuleTransformMap);

	}
	else
	{
		// 게임 시작 후 저장된 시간과 그 이후에 저장된 시간을 뺀값을 초기화
		float PktLagHistoryLength = PktLagLocationHistoryLinkedList.GetHead()->GetValue().PktLagServerTime - PktLagLocationHistoryLinkedList.GetTail()->GetValue().PktLagServerTime;
		while (PktLagHistoryLength > MaxRecordTime) // 4초 보다 크다면
		{
			// tail 위치를 삭제
			PktLagLocationHistoryLinkedList.RemoveNode(PktLagLocationHistoryLinkedList.GetTail());
			// 다시 시간 초기화
			PktLagHistoryLength = PktLagLocationHistoryLinkedList.GetHead()->GetValue().PktLagServerTime - PktLagLocationHistoryLinkedList.GetTail()->GetValue().PktLagServerTime;

		}
		FPktLagCapsuleTransformMap PktLagCapsuleTransformMap;
		PktLagSaveLocation(PktLagCapsuleTransformMap);
		PktLagLocationHistoryLinkedList.AddHead(PktLagCapsuleTransformMap);

		// debug
		PktLagDebugCapsule(PktLagCapsuleTransformMap, FColor::Orange);
	}
	
}

void UPktLagComponent::PktLagSaveLocation(FPktLagCapsuleTransformMap& PktLagCapsuleTransformMap)
{
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;
	//PlayerController = PlayerController == nullptr ? Cast<AShooterPlayerController>(Controller)

	PktLagCapsuleTransformMap.PktLagServerTime = GetWorld()->GetTimeSeconds();

	// 매핑된 collision component를 위치, 회전, 지름, 절반 높이의 값을 가져와 잘 가져왔는지 debug를 작성
	for (auto& PktLagCapsule : Character->PktLagMappingComponent)
	{
		// Map 컨테이너를 사용하기에 key, value로 지정하여 요소 참조
		FPktLagCapsuleInfo PktLagCI;
		PktLagCI.PktLagCapsuleComponentLocation = PktLagCapsule.Value->GetComponentLocation();			// 위치
		PktLagCI.PktLagCapsuleComponentRotator = PktLagCapsule.Value->GetComponentRotation();			// 회전
		PktLagCI.PktLagCapsuleComponentRadius = PktLagCapsule.Value->GetScaledCapsuleRadius();			// 반지름
		PktLagCI.PktLagCapsuleComponentHalfHeight = PktLagCapsule.Value->GetScaledCapsuleHalfHeight();	// 절반 높이

		// 실질적으로 사용할 컨테이너에 저장
		PktLagCapsuleTransformMap.PktLagCapsuleInfo.Add(PktLagCapsule.Key, PktLagCI);

		

	}
}

void UPktLagComponent::PktLagDebugCapsule(FPktLagCapsuleTransformMap& PktLagCapsuleTransformMap, FColor Color)
{
	for (auto& DebugCapsule : PktLagCapsuleTransformMap.PktLagCapsuleInfo)
	{
		DrawDebugCapsule(
			GetWorld(), 
			DebugCapsule.Value.PktLagCapsuleComponentLocation, 
			DebugCapsule.Value.PktLagCapsuleComponentHalfHeight, 
			DebugCapsule.Value.PktLagCapsuleComponentRadius, 
			FQuat(DebugCapsule.Value.PktLagCapsuleComponentRotator), 
			Color, 
			false, 
			4.f);

		//DrawDebugCapsule(GetWorld(), DebugCapsule.Value.PktLagCapsuleComponentLocation, DebugCapsule.Value.PktLagCapsuleComponentScale., 0.2f, FQuat(DebugCapsule.Value.PktLagCapsuleComponentRotator), Color, true);

		//GEngine->AddOnScreenDebugMessage(3, 15.f, FColor::Orange, TEXT("Capsule Scale : %s"), *DebugCapsule.Value.PktLagCapsuleComponentScale.ToString());
		//DrawDebugCapsule(GetWorld(), DebugCapsule.Value.PktLagCapsuleComponentLocation, DebugCapsule.Value.PktLagCapsuleComponentScale.get)
		//Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

	}
}