// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSideRewindComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"


UServerSideRewindComponent::UServerSideRewindComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}

void UServerSideRewindComponent::BeginPlay()
{
	Super::BeginPlay();

	FServerSideRewindSnapshot ssr;
	SaveServerSideRewind(ssr);
	DebugSSR(ssr);
	
}

void UServerSideRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UServerSideRewindComponent::SaveServerSideRewind(FServerSideRewindSnapshot& serversiderewindsnpshot)
{
	// 캐릭터 콜리전 박스를 컨테이너에 채우기 위한 함수
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;
	
	if (Character)
	{
		// 게임 시작 후 월드 시간 가져와서 초기화 진행 (기본 게임 모드에 캐릭터 스폰 시작 하기 6초로 설정되어 있어서 디버그 진행해도 문제 없음)
		serversiderewindsnpshot.Time = GetWorld()->GetTimeSeconds();

		// 게임 시작 후 캐릭터 스폰 후 생성자에서 저장된 콜리전 박스 컨테이너 각각의 요소 값을 저장
		for (auto& fill : Character->HitBoxesMap)
		{
			// 위치, 회전, 크기 가져와서 초기화 
			FHitBoxInfo HitBoxInfo;
			HitBoxInfo.Location = fill.Value->GetComponentLocation();
			HitBoxInfo.Rotation = fill.Value->GetComponentRotation();
			HitBoxInfo.Extent = fill.Value->GetScaledBoxExtent();

			// ssr 컴포넌트에 컨테이너 값을 채우기
			serversiderewindsnpshot.HitBoxSnapshot.Add(fill.Key, HitBoxInfo);

		}
	}
}

void UServerSideRewindComponent::DebugSSR(FServerSideRewindSnapshot& serversiderewindsnpshot)
{
	for (auto& fill : serversiderewindsnpshot.HitBoxSnapshot)
	{
		DrawDebugBox(GetWorld(), fill.Value.Location, fill.Value.Extent, FQuat(fill.Value.Rotation), FColor::Orange, true);
	}
}
