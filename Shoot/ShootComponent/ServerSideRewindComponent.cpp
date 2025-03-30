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

	/*FServerSideRewindSnapshot ssr;
	SaveServerSideRewind(ssr);
	DebugSSR(ssr);*/
	
}

void UServerSideRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);



	// 요소 개수 비교
	if (FrameHistory.Num() <= 1)
	{
		// 첫번째 요소에 info 저장 후 head 추가
		SaveServerSideRewind(ServerSideRewindSnapshot);
		FrameHistory.AddHead(ServerSideRewindSnapshot); //list

	}
	else
	{
		// 처음으로 저장된 시간 값과 끝 시간 값을 뺀 값을 초기화
		float HistoryTime = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		
		// 그 이상 저장한다면 불필요한 서버 처리를 하는것이며 최악의 상태 서버 지연이 400ms 이상이라면 굳이 처리할 필요 없다 (현재 테스트중인 지연 시간은 400이다)
		while (HistoryTime > MaxRewindTime)
		{
			// head 와 tail의 시간 값이 3초 이상이라면  tail를 삭제하는 작업 
			FrameHistory.RemoveNode(FrameHistory.GetTail());

			// 삭제 후 새롭게 시간 초기화
			HistoryTime = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
			
		}

		// 첫 요소가 3초 이상 상당의 요소값이 저장 됐다면 새롭게 다시 info 값 저장 후 head 추가
		SaveServerSideRewind(ServerSideRewindSnapshot);
		FrameHistory.AddHead(ServerSideRewindSnapshot);

		// debug 
		DebugSSR(ServerSideRewindSnapshot);
	}
	

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
		DrawDebugBox(GetWorld(), fill.Value.Location, fill.Value.Extent, FQuat(fill.Value.Rotation), FColor::Orange, false, 2.f);
	}
}
