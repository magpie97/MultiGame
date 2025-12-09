// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingGameMode.h"
#include "Shoot/Character/BaseCharacter.h"	// 제거 되거나 생성되야할 캐릭터의 헤더를 가져와야한다
#include "Shoot/PlayerController/ShooterPlayerController.h"	// 제거된 캐릭터의 컨트롤러를 가져와야하기에 작성한다
#include "Kismet/GameplayStatics.h"		// getallactorsofclass 함수를 사용하기 위한 라이브러리
#include "GameFramework/PlayerStart.h"
#include "Shoot/ShooterPlayerState/ShooterPlayerState.h"
#include "Components/PanelWidget.h"
#include "Shoot/GameState/ShooterGameState.h"
#include "Shoot/HUD/ShooterHUD.h"
#include "Sound/SoundCue.h"

#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

namespace MatchState
{
	const FName WaitNextGame = FName("WaitNextGame");

}

AShootingGameMode::AShootingGameMode()
{
	/*
		첫 번째 플레이어가 로그인 즉, 맵에 입장 했을때 즉시 게임모드를 실행 시킬지 여부를 ReadyToStartMatch의 기본 동작에 영향을 주는 변수이다 (비트로 초기화한 변수임)
	*/
	bDelayedStart = true;
	//bDelayedStart = false; // test
	
	


}

void AShootingGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();

	
	
}

void AShootingGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// 액터는 티킹을 하지만 게임은 시작하지 않는다
	if (GetMatchState() == MatchState::WaitingToStart)
	{
		//GEngine->AddOnScreenDebugMessage(0, 15.f, FColor::Red, TEXT("Current GameMode State : WaitingToStart"));
		
		// 인게임 시작하기전 웨이팅 시간 - 게임 시작시 카운트 되는 시간값 을 구하고 countdowntime 변수를 초기화
		// 10초
		CountdownTime = WaitTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		//DisableInput(ShooterPlayerController);

		if (CountdownTime <= 0.f)
		{
			// 이 함수가 실행 될때 watingToStart에서 InProgress로 리턴한다 
			// 그래서 유저(컨트롤러)의 matchstate가 inprogress로 바뀌고나서 countdwon 실행
			StartMatch();

		}
	}
	else if (GetMatchState() == MatchState::InProgress)
	{
		//GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Blue, TEXT("Current GameMode State : InProgress"));

		// 게임이 진행후 끝날 시간 계산 
		// 게임 진행 시간이 0초라면 다른게임 모드로 변경  
		CountdownTime = WaitTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			SetMatchState(MatchState::WaitNextGame);
		}
	}
	else if (GetMatchState() == MatchState::WaitNextGame)	// 게임이 끝나고 새로운 게임을 시작할 matchstate
	{
		//GEngine->AddOnScreenDebugMessage(0, 5.f, FColor::Black, TEXT("Current GameMode State : WaitNextGame"));

		// 게임이 끝난 이후 다음 게임으로 넘어갈 시간
		CountdownTime = WaitNextGameTime + WaitTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		// 월드 시간을 느리게 설정  test
		UGameplayStatics::SetGlobalTimeDilation(this, 0.2f);

		if (CountdownTime <= 0.f)
		{
			//RestartGame();

			UWorld* world = GetWorld();
			if (world)
			{
				//bUseSeamlessTravel = true;   // 이거 때문에 버그 남  우선 사용 중지
				world->ServerTravel(FString("/Game/Maps/ShootMap?Listen"));
				//World'/Game/Maps/ShootMap.ShootMap'
			}

			

		}
	}

	// killstreak 기능



}

void AShootingGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// playercontroller 반복자 사용해서 유저 인원 수 전부 돌려 부모 클래스에 선언된 MatchState를 OnMatchStateSet 함수를 가져와 set 해준다
	// getplayercontrollerinterator 함수는 playercontroller의 배열을 리턴하는 함수로 level에 배치된 컨트롤러 즉, 폰의 수 만큼 가져와 배열에 정렬 수
	// 요소만큼 실행 후 증가시켜 반복문을 사용한다
	for (FConstPlayerControllerIterator it = GetWorld()->GetPlayerControllerIterator(); it; ++it)
	{
		AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(*it);
		
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState);				 
			
		}
		//FString s = FString::Printf(TEXT(" Current MatchState : %s "), *MatchState.ToString());
		//GEngine->AddOnScreenDebugMessage(3, 60.f, FColor::Red, s);
		
	}

	/*for (auto It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const TWeakObjectPtr<APlayerController> PlayerController = *It;
		if (const auto ShooterPlayerController = Cast<AShooterPlayerController>(PlayerController))
		{
			ShooterPlayerController->OnMatchStateSet(MatchState);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to call OnMatchStateSet on controller %s " "as it is not an instance of AShooterPlayerController"),*PlayerController.Get()->GetName());
		}
	}*/
}

void AShootingGameMode::PlayerDead(class ABaseCharacter* DeadCharacter, class AShooterPlayerController* VictimPlayerController, AShooterPlayerController* AttackerPlayerController)
{
	// player의 공격하는 상태와 피해 받는상태를 null 체크					
	// PlayerState  이건 해당 controller를 사용하는 플레이어의 상태를 복사하는 변수이며 controller 클래스 내부에 선언된 변수이다 
	// NPC가 아닌 플레이어만 해당됀다

	AShooterPlayerState* AttackerPlayerState = AttackerPlayerController ? Cast<AShooterPlayerState>(AttackerPlayerController->PlayerState) : nullptr;
	AShooterPlayerState* VictimPlayerState = VictimPlayerController ? Cast<AShooterPlayerState>(VictimPlayerController->PlayerState) : nullptr;
	AShooterGameState* ShooterGameState = GetGameState<AShooterGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && ShooterGameState)	// 공격자 상태와 피해자 상태가 같지 않을때
	{
		// 공격자가 피해자를 죽였을때 점수 10점 
		AttackerPlayerState->AddToScore(1.f);
		
		ShooterGameState->UpdateHighScore(AttackerPlayerState);




		// killstreak
		AttackerPlayerState->IncrementKillStreak(ResetTime);

		// 현재 killstreak 단계 가져오기
		int32 CurrentKillStreak = AttackerPlayerState->CurrentKillStreakCount;

		// AttackerPlayerController에게 Client RPC를 호출하며 정확한 단계 전달
		if (AttackerPlayerController)
		{
			AttackerPlayerController->ClientNotifyKillStreak(CurrentKillStreak);
		}


	}

	if (VictimPlayerState && VictimPlayerState != AttackerPlayerState)
	{
		VictimPlayerState->AddToDeathScore(1.f);
		//GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Yellow, TEXT("Lost 1 Point"));

	}

	if (DeadCharacter)
	{
		DeadCharacter->Dead();

	}

	// kill feed section   
	// 이 번복자는 level안에 있는 모든 컨트롤러를 가진 user의 killfeed를 사용하기 위한 작업
	for (FConstPlayerControllerIterator CPCI = GetWorld()->GetPlayerControllerIterator(); CPCI; ++CPCI)
	{
		AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(*CPCI);
		if (PlayerController && AttackerPlayerState && VictimPlayerState)
		{
			PlayerController->Announcement_KillFeed(AttackerPlayerState, VictimPlayerState);

		}
	}
}

void AShootingGameMode::PlayerRespawn(ACharacter* DeadCharacter, AController* DeadController)
{
	// 체크
	if (DeadCharacter == nullptr && DeadController == nullptr) return;

	if (DeadCharacter)
	{
		DeadCharacter->Reset();
		DeadCharacter->Destroy();
	}

	if (DeadController)
	{
		// getallactorsofclass 함수는 모든 액터를 가져오는 함수이고 배열을 사용하여 랜덤한 player start위치에 restartplayeratplayerstart를 사용하기 위해 랜덤값을 넣는다
		TArray<AActor*> Actors;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), Actors);
		int32 PlayerRandomSpawnLocation = FMath::RandRange(0, Actors.Num() - 1);
		RestartPlayerAtPlayerStart(DeadController, Actors[PlayerRandomSpawnLocation]);


	}
}