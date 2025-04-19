// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSideRewindComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/Weapon/Weapon.h"

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

	// info 저장 3초후 삭제 
	SaveSnapshot();
	

}

void UServerSideRewindComponent::SaveHitBoxInfo(FServerSideRewindSnapshot& serversiderewindsnpshot)
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

FServerSideRewindResult UServerSideRewindComponent::ServerSideRewind(ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	HitCharacter = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;
	FServerSideRewindSnapshot ServerSideRewindSnapshot;

	bool bNullCheck = HitCharacter->GetServerSideRewindComponent()->FrameHistory.GetHead() == nullptr || HitCharacter->GetServerSideRewindComponent()->FrameHistory.GetTail() == nullptr;

	if (bNullCheck) return FServerSideRewindResult();
	TDoubleLinkedList<FServerSideRewindSnapshot>& History = HitCharacter->GetServerSideRewindComponent()->FrameHistory;

	float OldTime = History.GetTail()->GetValue().Time;
	float NewTime = History.GetHead()->GetValue().Time;
	
	// 중요 핵심 코드--------------------------
	auto NextNode = History.GetHead();
	auto PrevNode = NextNode;

	while (PrevNode->GetValue().Time > HitTime) // 이전 노드의 시간 값이 hit time보다 크다면 
	{
		if (PrevNode->GetNextNode() == nullptr) break;

		PrevNode = PrevNode->GetNextNode();

		if (PrevNode->GetValue().Time > HitTime)
		{
			PrevNode = NextNode;
		}

	}
	// --------------------------


	//debug
	UE_LOG(LogTemp, Warning, TEXT("Oldest: %f"), OldTime);
	UE_LOG(LogTemp, Warning, TEXT("Latest: %f"), NewTime);
	UE_LOG(LogTemp, Warning, TEXT("Hit: %f"), HitTime);



	// 예외 처리 코드 ----------------------
	// 3(지연 300ms)초를 넘겼으니 불 필요한 지연 보상에 제외 (현재 테스트 환경은 400ms)
	if (OldTime > HitTime) { return FServerSideRewindResult(); }

	// 말도 안되는 상황이지만 혹시 모르는 상황에 대비하여 작성  (데미지 처리 할 시간보다 앞선 시간에 info 저장된다는게 말이 안되긴 하다)
	if (NewTime <= HitTime) { ServerSideRewindSnapshot = History.GetHead()->GetValue(); }
	if (OldTime <= HitTime) { ServerSideRewindSnapshot = History.GetTail()->GetValue(); }
	if (PrevNode->GetValue().Time == HitTime){ ServerSideRewindSnapshot = PrevNode->GetValue(); }
	// -----------------------------------

	if (bNullCheck)
	{
		return FServerSideRewindResult();
	}
	

	return CheckHitBody(ServerSideRewindSnapshot, HitCharacter, TraceStart, HitLocation);
}

FServerSideRewindResult UServerSideRewindComponent::CheckHitBody(FServerSideRewindSnapshot& SSRS, ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if(HitCharacter == nullptr) return FServerSideRewindResult();

	FServerSideRewindSnapshot ServerSideRewindSnapshot;
	// 임시로 hit 한 위치를 저장 
	SaveTempBoxLocation(HitCharacter, ServerSideRewindSnapshot);    // 임시로 hit 위치 저장
	MoveTempBoxLocation(HitCharacter, SSRS);						// 임시로 저장한 위치에 콜리전 박스 위치, 크기를 옮겨서 저장 
	EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	// 기존 박스의 콜리전을 no collision에서 block 으로 변경 
	UBoxComponent* boxcomponent = HitCharacter->HitBoxesMap[FName("head")];
	boxcomponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxcomponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


	// linetrace 섹션
	FHitResult hitresult;
	FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;

	UWorld* world = GetWorld();
	if (world)
	{
		world->LineTraceSingleByChannel(hitresult, TraceStart, TraceEnd, ECollisionChannel::ECC_PhysicsBody);
		
		if (hitresult.bBlockingHit)
		{
			// 콜리전 상태를 no collision 상태로 변경 
			ResetBoxLocation(HitCharacter, ServerSideRewindSnapshot);

			EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);

			// head, body, check 순서
			return FServerSideRewindResult{ true, true };

		}
		else
		{
			for (auto& reset : HitCharacter->HitBoxesMap)
			{
				reset.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				reset.Value->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);


			}

			world->LineTraceSingleByChannel(hitresult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
			if (hitresult.bBlockingHit)
			{
				// 콜리전 상태를 no collision 상태로 변경 
				ResetBoxLocation(HitCharacter, ServerSideRewindSnapshot);

				EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);

				// head, body, check 순서
				return FServerSideRewindResult{ false, true };

			}

		}
	}
	// 콜리전 상태를 no collision 상태로 변경 
	ResetBoxLocation(HitCharacter, ServerSideRewindSnapshot);

	EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);

	// head, body, check 순서
	return FServerSideRewindResult{ false, false };


}

void UServerSideRewindComponent::SaveTempBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS)
{
	if (HitCharacter == nullptr) return;

	// 일시 적으로 캐릭터의 hit 위치를 저장
	for (auto& fill : HitCharacter->HitBoxesMap)
	{
		FHitBoxInfo hitboxinfo;
		hitboxinfo.Location = fill.Value->GetComponentLocation();
		hitboxinfo.Rotation = fill.Value->GetComponentRotation();
		hitboxinfo.Extent = fill.Value->GetScaledBoxExtent();
		SSRS.HitBoxSnapshot.Add(fill.Key, hitboxinfo);

	}
}

void UServerSideRewindComponent::MoveTempBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS)
{
	if (HitCharacter == nullptr) return;

	// hit 위치로 콜리전 박스 위치, 크기를 옮겨 저장
	for (auto& setlocation : HitCharacter->HitBoxesMap)
	{
		setlocation.Value->SetWorldLocation(SSRS.HitBoxSnapshot[setlocation.Key].Location);
		setlocation.Value->SetWorldRotation(SSRS.HitBoxSnapshot[setlocation.Key].Rotation);
		setlocation.Value->SetBoxExtent(SSRS.HitBoxSnapshot[setlocation.Key].Extent);
		
	}
}

void UServerSideRewindComponent::ResetBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS)
{
	if (HitCharacter == nullptr) return;

	for (auto& reset : HitCharacter->HitBoxesMap)
	{
		FHitBoxInfo hitboxinfo;
		reset.Value->SetWorldLocation(SSRS.HitBoxSnapshot[reset.Key].Location);
		reset.Value->SetWorldRotation(SSRS.HitBoxSnapshot[reset.Key].Rotation);
		reset.Value->SetBoxExtent(SSRS.HitBoxSnapshot[reset.Key].Extent);
		reset.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
}

void UServerSideRewindComponent::EnableChracterMeshCollision(ABaseCharacter* HitCharacter, ECollisionEnabled::Type Collision)
{
	HitCharacter = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;

	if (HitCharacter)
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(Collision);

	}
}

void UServerSideRewindComponent::ServerDamageRequest_Implementation(ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* Damage)
{
	// 서버 먼저 데미지 처리
	FServerSideRewindResult CheckDamage = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);

	if (Character && CheckDamage.bCheckHit)
	{
		UGameplayStatics::ApplyDamage(HitCharacter, Damage->GetDamage(), Character->Controller, Damage, UDamageType::StaticClass());

	}
}

void UServerSideRewindComponent::SaveSnapshot()
{
	// 서버만 사용
	if (Character == nullptr || !Character->HasAuthority()) return;

	FServerSideRewindSnapshot ServerSideRewindSnapshot;

	// 요소 개수 비교
	if (FrameHistory.Num() <= 1)
	{
		// 첫번째 요소에 info 저장 후 head 추가
		SaveHitBoxInfo(ServerSideRewindSnapshot);
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
		SaveHitBoxInfo(ServerSideRewindSnapshot);
		FrameHistory.AddHead(ServerSideRewindSnapshot);

		// debug 
		//DebugSSR(ServerSideRewindSnapshot);
	}

}