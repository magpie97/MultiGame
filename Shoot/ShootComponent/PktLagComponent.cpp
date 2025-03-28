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

	// debug
	DebugServerRewide();
}

void UPktLagComponent::DebugServerRewide()
{
	// 0번째 키 위치에 (head) 접근하여 확인 
	if (PktLagLocationHistoryLinkedList.Num() <= 1)
	{
		// 캡슐 콜리전 구조체 모음
		FPktLagCapsuleTransformMap PktLagCapsuleTransformMap;

		// 캡슐 컴포넌트 transform 값을 저장 호출
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

	// 게임 시작 후 시간 변수 초기화
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
			1.f);

	}
}

void UPktLagComponent::ServerSideRewinde(class ABaseCharacter* HitCharacter, FVector_NetQuantize& TraceStart, FVector_NetQuantize& TraceEnd, float HitTime)
{
	bool bNullCheck =
		HitCharacter == nullptr ||
		HitCharacter->GetPktLagComponent() == nullptr ||
		HitCharacter->GetPktLagComponent()->PktLagLocationHistoryLinkedList.GetHead() == nullptr ||
		HitCharacter->GetPktLagComponent()->PktLagLocationHistoryLinkedList.GetTail() == nullptr;

	// 보간 처리
	bool bShouldInterpolate = true;

	// 캐릭터의 캡슐 콜리전 컴포넌트 위치와 시간을 저장한 구조체
	FPktLagCapsuleTransformMap FrameToCheck;

	// 이중 연결 리스트 생성 타입은 위치와 시간 구조체를 초기화
	TDoubleLinkedList<FPktLagCapsuleTransformMap>& LocationHistory = HitCharacter->GetPktLagComponent()->PktLagLocationHistoryLinkedList;
	float OldTime = LocationHistory.GetTail()->GetValue().PktLagServerTime;
	float NewTime = LocationHistory.GetHead()->GetValue().PktLagServerTime;

	// 예외 처리
	if (OldTime > HitTime)
	{
		// 리와인드 하기엔 지연시간이 너무 길었다
		return;
	}

	// Hittime과 새롭게 초기화 된 시간 값이 같다면
	if (NewTime == HitTime)
	{
		// 
		FrameToCheck = LocationHistory.GetTail()->GetValue();
		bShouldInterpolate = false;
	}

	if (NewTime <= OldTime)
	{
		FrameToCheck = LocationHistory.GetHead()->GetValue();
		bShouldInterpolate = false;

	}

	TDoubleLinkedList<FPktLagCapsuleTransformMap>::TDoubleLinkedListNode* Younger = LocationHistory.GetHead();
	TDoubleLinkedList<FPktLagCapsuleTransformMap>::TDoubleLinkedListNode* Older = Younger;

	while (Older->GetValue().PktLagServerTime > HitTime)
	{
		if (Older->GetNextNode() == nullptr) break;
		Older = Older->GetNextNode();
		if (Older->GetValue().PktLagServerTime > HitTime)
		{
			Younger = Older;
		}

	}

	// 그럴 일 없지만 혹시 모를 상황에 대비할 기능  확인 필요
	if (Older->GetValue().PktLagServerTime == HitTime)
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}

	if (bShouldInterpolate)
	{
		// 보간 처리 할 알고리즘 필요

	}

	if (bNullCheck) return;

}

FPktLagCapsuleTransformMap UPktLagComponent::InterpBetweenFrames(FPktLagCapsuleTransformMap& OlderFrame, FPktLagCapsuleTransformMap& NewerFrame, float HitTime)
{
	// 새로 생긴 시간 값와 이전에 생긴 시간 값을 저장
	float Distance = NewerFrame.PktLagServerTime - OlderFrame.PktLagServerTime;

	// 보간의 중간 값을 가져와 공백을 처리할 변수
	float InterpFraction = FMath::Clamp((HitTime - OlderFrame.PktLagServerTime) / Distance, 0.f, 1.f);
	
	FPktLagCapsuleTransformMap PktLagCapsuleTransformMap;
	PktLagCapsuleTransformMap.PktLagServerTime = HitTime;

	for (auto Pair : NewerFrame.PktLagCapsuleInfo)
	{
		FName& CapsuleName = Pair.Key;

		FPktLagCapsuleInfo& NewCapsuleInfo = OlderFrame.PktLagCapsuleInfo[CapsuleName];
		FPktLagCapsuleInfo& OldCapsuleInfo = NewerFrame.PktLagCapsuleInfo[CapsuleName];

		FPktLagCapsuleInfo InterpCapsuleInfo;
		InterpCapsuleInfo.PktLagCapsuleComponentLocation = FMath::VInterpTo(OldCapsuleInfo.PktLagCapsuleComponentLocation, NewCapsuleInfo.PktLagCapsuleComponentLocation, 1.f, InterpFraction);
		InterpCapsuleInfo.PktLagCapsuleComponentRotator = FMath::RInterpTo(OldCapsuleInfo.PktLagCapsuleComponentRotator, NewCapsuleInfo.PktLagCapsuleComponentRotator, 1.f, InterpFraction);
		InterpCapsuleInfo.PktLagCapsuleComponentRadius = NewCapsuleInfo.PktLagCapsuleComponentRadius;
		InterpCapsuleInfo.PktLagCapsuleComponentHalfHeight = NewCapsuleInfo.PktLagCapsuleComponentHalfHeight;

		PktLagCapsuleTransformMap.PktLagCapsuleInfo.Add(CapsuleName, InterpCapsuleInfo);

	}


	return PktLagCapsuleTransformMap;
}