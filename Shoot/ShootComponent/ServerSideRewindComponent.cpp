// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerSideRewindComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/Weapon/Weapon.h"

#include "DrawDebugHelpers.h"
#include "Shoot/Shoot.h"
#include "Templates/Tuple.h"



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



void UServerSideRewindComponent::SaveHitBoxInfo(FServerSideRewindSnapshot& serversiderewindsnpshot)
{
	// 캐릭터 콜리전 박스를 컨테이너에 채우기 위한 함수
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetOwner()) : Character;
	
	if (Character)
	{
		// 게임 시작 후 월드 시간 가져와서 초기화 진행 (기본 게임 모드에 캐릭터 스폰 시작 하기 6초로 설정되어 있어서 디버그 진행해도 문제 없음)
		serversiderewindsnpshot.Time = GetWorld()->GetTimeSeconds();
		serversiderewindsnpshot.Character = Character;

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



FServerSideRewindResult UServerSideRewindComponent::ProjectileServerSideRewind(ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FServerSideRewindSnapshot SnapshotCheck = GetFrameToCheck(HitCharacter, HitTime);

	return CheckProjectileHitBody(SnapshotCheck, HitCharacter, TraceStart, InitialVelocity, HitTime); // 문제
}

FServerSideRewindSnapshot UServerSideRewindComponent::GetFrameToCheck(ABaseCharacter* HitCharacter, float HitTime)
{
	bool bNullCheck = HitCharacter == nullptr || HitCharacter->GetServerSideRewindComponent() || HitCharacter->GetServerSideRewindComponent()->FrameHistory.GetHead() == nullptr || HitCharacter->GetServerSideRewindComponent()->FrameHistory.GetTail() == nullptr;

	FServerSideRewindSnapshot ServerSideRewindSnapshot;

	if (bNullCheck) return FServerSideRewindSnapshot();

	TDoubleLinkedList<FServerSideRewindSnapshot>& History = HitCharacter->GetServerSideRewindComponent()->FrameHistory;

	const float OldTime = History.GetTail()->GetValue().Time;
	const float NewTime = History.GetHead()->GetValue().Time;

	if (OldTime > HitTime)
	{
		return FServerSideRewindSnapshot();
	}

	if (OldTime == HitTime)
	{
		ServerSideRewindSnapshot = History.GetTail()->GetValue();
	}

	if (NewTime <= HitTime)
	{
		ServerSideRewindSnapshot = History.GetHead()->GetValue();
	}

	// 중요 핵심 코드--------------------------
	auto NextNode = History.GetHead();
	auto PrevNode = NextNode;

	while (PrevNode->GetValue().Time > HitTime) // 이전 노드의 시간 값이 hit time보다 크다면 
	{
		if (PrevNode->GetNextNode() == nullptr) break;

		PrevNode = PrevNode->GetNextNode();

		if (PrevNode->GetValue().Time > HitTime)
		{
			NextNode = PrevNode;
		}
	}
	// --------------------------


	//debug
	UE_LOG(LogTemp, Warning, TEXT("Oldest: %f"), OldTime);
	UE_LOG(LogTemp, Warning, TEXT("Latest: %f"), NewTime);
	UE_LOG(LogTemp, Warning, TEXT("Hit: %f"), HitTime);

	if (PrevNode->GetValue().Time == HitTime)
	{
		ServerSideRewindSnapshot = PrevNode->GetValue();
	}

	// 예외 처리 코드 ----------------------  임시 주석 test
	// 3(지연 300ms)초를 넘겼으니 불 필요한 지연 보상에 제외 (현재 테스트 환경은 400ms)
	//if (OldTime > HitTime) { return FServerSideRewindResult(); }

	// 말도 안되는 상황이지만 혹시 모르는 상황에 대비하여 작성  (데미지 처리 할 시간보다 앞선 시간에 info 저장된다는게 말이 안되긴 하다)
	//if (NewTime <= HitTime) { ServerSideRewindSnapshot = History.GetHead()->GetValue(); }
	//if (OldTime <= HitTime) { ServerSideRewindSnapshot = History.GetTail()->GetValue(); }
	//if (PrevNode->GetValue().Time == HitTime){ ServerSideRewindSnapshot = PrevNode->GetValue(); }
	// -----------------------------------

	// 임시 주석
	/*if (bNullCheck)
	{
		return FServerSideRewindResult();
	}*/

	ServerSideRewindSnapshot.Character = HitCharacter;

	return ServerSideRewindSnapshot;
}

FServerSideRewindResult UServerSideRewindComponent::CheckProjectileHitBody(FServerSideRewindSnapshot& SSRS, ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FServerSideRewindSnapshot ServerSideRewindSnapshot;
	// 임시로 hit 한 위치를 저장 
	SaveTempBoxLocation(HitCharacter, ServerSideRewindSnapshot);    // 임시로 hit 위치 저장
	MoveTempBoxLocation(HitCharacter, SSRS);						// 임시로 저장한 위치에 콜리전 박스 위치, 크기를 옮겨서 저장 
	EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision); // 문제

	// 기존 박스의 콜리전을 no collision에서 block 으로 변경 
	class UBoxComponent* boxcomponent = HitCharacter->HitBoxesMap[FName("head")];
	boxcomponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	boxcomponent->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);

	// todo: projectile 궤적 생성
	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithCollision = true;
	PathParams.MaxSimTime = MaxRewindTime;
	PathParams.LaunchVelocity = InitialVelocity;
	PathParams.StartLocation = TraceStart;
	PathParams.SimFrequency = 15.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(GetOwner());
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;

	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
	

	FHitResult HitResult;
	if (PathResult.HitResult.bBlockingHit)
	{
		if (PathResult.HitResult.Component.IsValid())
		{
			UBoxComponent* Box = Cast<UBoxComponent>(PathResult.HitResult.Component);
			if (Box)
			{
				DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Red, false, 15.f);

			}
		}

		// 콜리전 상태를 no collision 상태로 변경 
		ResetBoxLocation(HitCharacter, ServerSideRewindSnapshot);

		EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);

		// chek, Hit 순서
		return FServerSideRewindResult{ true, true };

	}
	else
	{
		// head 소켓의 콜리전이 작동 안한다면 
		for (auto& snapshots : HitCharacter->HitBoxesMap)
		{
			if (snapshots.Value != nullptr)
			{
				snapshots.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				snapshots.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);

			}
		}

		UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);

		if (PathResult.HitResult.bBlockingHit)
		{
			// debug
			if (PathResult.HitResult.Component.IsValid())
			{
				UBoxComponent* Box = Cast<UBoxComponent>(PathResult.HitResult.Component);
				if (Box)
				{
					DrawDebugBox(GetWorld(), Box->GetComponentLocation(), Box->GetScaledBoxExtent(), FQuat(Box->GetComponentRotation()), FColor::Purple, false, 15.f);
				}
			}

			// 콜리전 상태를 no collision 상태로 변경 
			ResetBoxLocation(HitCharacter, ServerSideRewindSnapshot);

			EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);

			// chek, Hit 순서
			return FServerSideRewindResult{ true, false };


		}
	}


	// 콜리전 상태를 no collision 상태로 변경 
	ResetBoxLocation(HitCharacter, ServerSideRewindSnapshot);

	EnableChracterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);

	// chek, Hit 순서
	return FServerSideRewindResult{ false, false };

}

void UServerSideRewindComponent::SaveTempBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS)
{
	if (HitCharacter == nullptr) return;

	// 일시 적으로 캐릭터의 hit 위치를 저장
	for (auto& fill : HitCharacter->HitBoxesMap)
	{
		if (fill.Value != nullptr)
		{
			FHitBoxInfo hitboxinfo;
			hitboxinfo.Location = fill.Value->GetComponentLocation();
			hitboxinfo.Rotation = fill.Value->GetComponentRotation();
			hitboxinfo.Extent = fill.Value->GetScaledBoxExtent();
			SSRS.HitBoxSnapshot.Add(fill.Key, hitboxinfo);
		}
		

	}
}

void UServerSideRewindComponent::MoveTempBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS)
{
	if (HitCharacter == nullptr) return;

	for (TTuple<FName, UBoxComponent*>& HitBox : HitCharacter->HitBoxesMap)
	{
		if (HitBox.Value != nullptr)
		{
			const FHitBoxInfo* BoxInfo = SSRS.HitBoxSnapshot.Find(HitBox.Key);

			if (BoxInfo)
			{
				HitBox.Value->SetWorldLocation(SSRS.HitBoxSnapshot[HitBox.Key].Location);
				HitBox.Value->SetWorldRotation(SSRS.HitBoxSnapshot[HitBox.Key].Rotation);
				HitBox.Value->SetBoxExtent(SSRS.HitBoxSnapshot[HitBox.Key].Extent);


			}
		}
	}


	// hit 위치로 콜리전 박스 위치, 크기를 옮겨 저장
	//for (auto& setlocation : HitCharacter->HitBoxesMap)
	//{
	//	if (setlocation.Value != nullptr)
	//	{
	//		setlocation.Value->SetWorldLocation(SSRS.HitBoxSnapshot[setlocation.Key].Location);	// 문제
	//		setlocation.Value->SetWorldRotation(SSRS.HitBoxSnapshot[setlocation.Key].Rotation);
	//		setlocation.Value->SetBoxExtent(SSRS.HitBoxSnapshot[setlocation.Key].Extent);

	//	}
	//	
	//}
}

void UServerSideRewindComponent::ResetBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS)
{
	if (HitCharacter == nullptr) return;

	for (TTuple<FName, UBoxComponent*>& HitBox : HitCharacter->HitBoxesMap)
	{
		if (HitBox.Value != nullptr)
		{
			const FHitBoxInfo* BoxInfo = SSRS.HitBoxSnapshot.Find(HitBox.Key);

			if (BoxInfo)
			{
				HitBox.Value->SetWorldLocation(SSRS.HitBoxSnapshot[HitBox.Key].Location);
				HitBox.Value->SetWorldRotation(SSRS.HitBoxSnapshot[HitBox.Key].Rotation);
				HitBox.Value->SetBoxExtent(SSRS.HitBoxSnapshot[HitBox.Key].Extent);
				HitBox.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

		}
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

void UServerSideRewindComponent::ServerProjectileDamageRequest_Implementation(ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FServerSideRewindResult CheckServerDamage = ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime);

	if (Character && HitCharacter && CheckServerDamage.bCheckHit)
	{
		UGameplayStatics::ApplyDamage(HitCharacter, Character->GetEquippedWeapon()->GetDamage(), Character->Controller, Character->GetEquippedWeapon(), UDamageType::StaticClass()); // 여기서 문제 생김

	}

}


void UServerSideRewindComponent::SaveSnapshot()
{
	// 서버만 사용
	if (Character == nullptr || !Character->HasAuthority()) return;

	//FServerSideRewindSnapshot ServerSideRewindSnapshot;

	// 요소 개수 비교
	if (FrameHistory.Num() <= 1)
	{
		FServerSideRewindSnapshot ServerSideRewindSnapshot;
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

		FServerSideRewindSnapshot ServerSideRewindSnapshot;

		// 첫 요소가 3초 이상 상당의 요소값이 저장 됐다면 새롭게 다시 info 값 저장 후 head 추가
		SaveHitBoxInfo(ServerSideRewindSnapshot);
		FrameHistory.AddHead(ServerSideRewindSnapshot);

		// debug 
		//DebugSSR(ServerSideRewindSnapshot);
	}

}

void UServerSideRewindComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// info 저장 3초후 삭제 
	SaveSnapshot();

}