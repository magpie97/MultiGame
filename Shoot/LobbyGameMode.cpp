// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "ShootGameInstance.h"
#include "Kismet/GameplayStatics.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/*
	// 플레이어 들어올때 증가
	++NumberOfPlayers;

	// 플레이어가 총 3명이 됐을때 servertravel 실행 지금은 필요 없을 듯하다
	if (NumberOfPlayers >= 3)
	{
		UWorld* World = GetWorld();
		if (World != nullptr) return;
		if (!ensure(World != nullptr)) return;

		// 매끄럽게 호스트 레벨에 들어가기
		bUseSeamlessTravel = true;

		World->ServerTravel("/Game/Maps/ShootMap?listen");
	}
	*/
	

	// %%% 현재 배치 파일로 실행시 잘 작동하지만 건이랑 테스트한 스팀 연동으로 리슨 서버를 열었을때 2명이 되어도 다름 레벨로 옮겨지지가 않는다
	// 해결해야 함   됀듯함 

	NumberOfPlayers = GameState->PlayerArray.Num();
	
	//++NumberOfPlayers;

	ShootGameInstance = ShootGameInstance == nullptr ? Cast<UShootGameInstance>(GetGameInstance()) : ShootGameInstance;	

	if (ShootGameInstance && NumberOfPlayers/* == ShootGameInstance->GetNumPublicConnections()*/)	// NumberOfPlayers 사용함
	{
		StartDelay();

	}
}

void ALobbyGameMode::StartDelay()
{
	GetWorld()->GetTimerManager().SetTimer(StartDelayHandle, this, &ALobbyGameMode::TravelShootMap, DelayTime);

}

void ALobbyGameMode::TravelShootMap()
{
	//UE_LOG(LogTemp, Error, TEXT("ShootGameInstance is Not NULL"));

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	// 매끄럽게 호스트 레벨에 이동
	bUseSeamlessTravel = true;

	// World'/Game/Maps/ShootMap.ShootMap'
	World->ServerTravel(FString("/Game/Maps/ShootMap?listen"));

	GetWorld()->GetTimerManager().ClearTimer(StartDelayHandle);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	// 플레이어가 나갈때 감소
	--NumberOfPlayers;
}