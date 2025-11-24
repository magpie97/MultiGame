// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "Shoot/HUD/ShooterHUD.h"
#include "Shoot/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Shoot/HUD/PlayerScoreWidget.h"
#include "Net/UnrealNetwork.h"
#include "Shoot/ShootingGameMode.h"
#include "Shoot/HUD/ShooterHUD.h"
#include "Shoot/HUD/PrepareToAttack.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/GameState/ShooterGameState.h"
#include "Shoot/ShooterPlayerState/ShooterPlayerState.h"
#include "Shoot/ShootComponent/CombatComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Shoot/ShootGameInstance.h"
#include "Shoot/HUD/ChatSystem.h"
#include "Shoot/ShooterPlayerState/ShooterPlayerState.h"
#include "Sound/SoundCue.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"


AShooterPlayerController::AShooterPlayerController()
{
	// SoundCue'/Game/Blueprint/Sound/KillStreak/FirstKill_Trimmed.FirstKill_Trimmed'
	//static ConstructorHelpers::FClassFinder<AShooterPlayerController> BPKillStreakFirstKillSound(TEXT("/Game/Blueprint/Sound/KillStreak/FirstKill_Trimmed"));
	//if (BPKillStreakFirstKillSound.Succeeded() && BPKillStreakFirstKillSound.Class != nullptr)
	//{
	//	FirstKill = BPKillStreakFirstKillSound.Class;
	//}

	//// SoundCue'/Game/Blueprint/Sound/KillStreak/DoubleKill_Trimmed.DoubleKill_Trimmed'
	//static ConstructorHelpers::FClassFinder<AShooterPlayerController> BPKillStreakDoubleKillSound(TEXT("/Game/Blueprint/Sound/KillStreak/DoubleKill_Trimmed"));
	//if (BPKillStreakDoubleKillSound.Succeeded() && BPKillStreakDoubleKillSound.Class != nullptr)
	//{
	//	FirstKill = BPKillStreakDoubleKillSound.Class;
	//}



}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// GetHUD()함수의 리턴 값인 myhud 라는 매개변수를 ASooterHUD 타입으로 형변환 시킨다
	ShooterHUD = Cast<AShooterHUD>(GetHUD());

	ServerCheckMatchState();


	// 게임 시작 후 채팅 하기전 마우스 포인터 끄기
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());



}

void AShooterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(FName("Chat"), EInputEvent::IE_Pressed, this, &AShooterPlayerController::FocusChatInputText);

}

void AShooterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AShooterPlayerController, MatchState);

}

void AShooterPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(aPawn);
	if (BaseCharacter)
	{
		SetHUDHealth(BaseCharacter->GetHealth(), BaseCharacter->GetMaxHealth());
	}
	

}

void AShooterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// test
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	// 매치 타이머 함수
	SetHUDTime();

	CheckTimeSync(DeltaTime);

	PollInit();

	// controller 클래스에 정의된걸 확인 했음
	PlayerState = PlayerState == nullptr ? GetPlayerState<AShooterPlayerState>() : PlayerState;

	if (PlayerState)
	{
		GEngine->AddOnScreenDebugMessage(4, 1.f, FColor::White, FString::Printf(TEXT("Ping : %d"), PlayerState->GetPing() * 4));
		
	}
}

void AShooterPlayerController::CheckTimeSync(float DeltaTime)
{
	// tick deltatime 변수 사용하여 서버와 클라의 시간을 
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		SRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}

}

void AShooterPlayerController::ServerCheckMatchState_Implementation()
{
	ShootingGameMode = Cast<AShootingGameMode>(UGameplayStatics::GetGameMode(this));

	if (ShootingGameMode)
	{
		MatchState = ShootingGameMode->GetMatchState();
		WatingTime = ShootingGameMode->WaitTime;
		MatchTime = ShootingGameMode->MatchTime;
		WaitNextGameTime = ShootingGameMode->WaitNextGameTime;
		LevelStartingTime = ShootingGameMode->LevelStartingTime;

		ClientJoinMidGame(MatchState, WatingTime, MatchTime, LevelStartingTime, WaitNextGameTime);

	}
}

void AShooterPlayerController::ClientJoinMidGame_Implementation(FName MS, float WT, float MT, float LST, float WNGT)
{
	MatchState = MS;
	WatingTime = WT;
	MatchTime = MT;
	LevelStartingTime = LST;
	WaitNextGameTime = WNGT;

	OnMatchStateSet(MatchState);

	if (ShooterHUD && MatchState == MatchState::WaitingToStart)
	{
		ShooterHUD->AddPrepareToAttack();
	}
}

// HUD 초기화
void AShooterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (ShooterHUD && ShooterHUD->CharacterOverlay)
		{
			CharacterOverlay = ShooterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
			    if (bInitHUDCurrentHealth) { SetHUDHealth(HUDCurrentHealth, HUDMaxHealth); }
				if (bInitHUDKillScore)     { SetHUDKillScore(HUDKillScore);                }
				if (bInitHUDDeadthScore)   { SetHUDDeathScore(HUDDeathScore);              }
				if (bInitHUDCarriedAmmo)   { SetHUDCarriedAmmo(HUDCarriedAmmo);            }
				if (bInitHUDWeaponAmmo)    { SetHUDWeaponAmmo(HUDWeaponAmmo);              }
				//if (bInitHUDGrenade) { SetHUDCarriedGrenade(HUDGrenade); }// test
				

				ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetPawn());
				if (BaseCharacter && BaseCharacter->GetCombat())
				{
					if (bInitHUDGrenade)
					{
						SetHUDCarriedGrenade(BaseCharacter->GetCombat()->GetCarriedGrenade());
					}

				}
			}
		}
	}
}

void AShooterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	// check
	bool bHUDValid = ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->HealthBar && ShooterHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		// 캐릭터의 채력을 백분율을 구한다
		const float HealthPersent = Health / MaxHealth;
		ShooterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPersent);

		// 나중에 health를 float 형 소수점 2째 자리까지 표현하는걸로 수정해보자
		FString HealthText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Health));		// default 
		ShooterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));

	}
	else
	{
		bInitHUDCurrentHealth = true;
		HUDCurrentHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AShooterPlayerController::SetHUDKillScore(float KillScore)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool bHUDVaild = ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->PlayerKillScore;

	if (bHUDVaild)
	{
		FString killscore = FString::Printf(TEXT("%d"), FMath::FloorToInt(KillScore));
		ShooterHUD->CharacterOverlay->PlayerKillScore->SetText(FText::FromString(killscore));

	}
	else
	{
		bInitCharacterOverlay = true;
		HUDKillScore = KillScore;
	}
}

void AShooterPlayerController::SetHUDDeathScore(int32 DeathScore)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool bHUDVaild = ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->PlayerDeathScore;

	if (bHUDVaild)
	{
		FString deathscore = FString::Printf(TEXT("%d"), DeathScore);
		ShooterHUD->CharacterOverlay->PlayerDeathScore->SetText(FText::FromString(deathscore));

	}
	else
	{
		bInitCharacterOverlay = true;
		HUDDeathScore = DeathScore;
	}
}

void AShooterPlayerController::ClientNotifyKillStreak_Implementation(int32 KillCount)
{
	// todo : 킬 카운트사운드를 5호 이후로 킬이 없다면 초기화

	kc = KillCount;

	switch (kc)
	{
		case 1:
		{
			UGameplayStatics::PlaySound2D(this, FirstKill);
			UGameplayStatics::PlaySound2D(this, FirstKill_Voice);

			GEngine->AddOnScreenDebugMessage(5, 3.f, FColor::Red, FString::Printf(TEXT("%d"), kc));
			
			break;
		}
		case 2:
		{
			UGameplayStatics::PlaySound2D(this, DoubleKill);
			UGameplayStatics::PlaySound2D(this, DoubleKill_Voice);

			break;
		}
		case 3:
		{
			UGameplayStatics::PlaySound2D(this, TripleKill);
			UGameplayStatics::PlaySound2D(this, TripleKill_Voice);

			break;
		}
		case 4:
		{
			UGameplayStatics::PlaySound2D(this, QuadraKill);
			UGameplayStatics::PlaySound2D(this, QuadraKill_Voice);

			break;
		}
		case 5:
		{
			UGameplayStatics::PlaySound2D(this, Pentakill);
			UGameplayStatics::PlaySound2D(this, Pentakill_Voice);

			break;
		}
	}

	if (KillCount > 5 && Pentakill)
	{
		UGameplayStatics::PlaySound2D(this, Pentakill);
		UGameplayStatics::PlaySound2D(this, Pentakill_Voice);
	}

	
	// 2. 위젯 생성 및 표시 (로컬 화면에만 보임)
	/*if (KillCount >= 3 && KillstreakWidgetClass)
	{
	
	}*/


}

void AShooterPlayerController::ResetKillStreak()
{
	// todo 일정 시간 이후 킬이 없다면 다시 초기화 5 초 시간이면 충분

}

void AShooterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool bHUDVaild = ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDVaild)
	{
		FString TextAmmo = FString::Printf(TEXT("%d"), Ammo);
		ShooterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(TextAmmo));

	}
	else
	{
		bInitHUDWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;

	}
}

void AShooterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool bHUDVaild = ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDVaild)
	{
		FString TextCarriedAmmo = FString::Printf(TEXT("%d"), Ammo);
		ShooterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(TextCarriedAmmo));

	}
	else
	{
		bInitHUDCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;

	}
}

void AShooterPlayerController::SetHUDCarriedGrenade(int32 Grenade)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool bHUDVaild = ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->GrenadeAmount;

	if (bHUDVaild)
	{
		FString TextCarriedGrenade = FString::Printf(TEXT("%d"), Grenade);
		ShooterHUD->CharacterOverlay->GrenadeAmount->SetText(FText::FromString(TextCarriedGrenade));

	}
	else
	{
		bInitHUDGrenade = true;
		HUDGrenade = Grenade;
	}
}

// 게임 시작 InProgress 상태
void AShooterPlayerController::SetHUDMatchCountdown(float Countdowntime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool bHUDVaild = ShooterHUD && ShooterHUD->CharacterOverlay && ShooterHUD->CharacterOverlay->MatchCountdown;

	//float count = 0.f;

	if (bHUDVaild)
	{
		if (Countdowntime < 0.f)
		{
			ShooterHUD->CharacterOverlay->MatchCountdown->SetText(FText());
			//UE_LOG(LogTemp, Warning, TEXT("Countdowntime is %f"), Countdowntime);
			
			return;
		}

		// 실수인 값을 (예: 8.4, 6.5을 8 또는 6으로) int형 변수로 변환가능한 함수 FloorToInt
		// 매개변수엔 180초 설정 해둠 
		int32 Minutes = FMath::FloorToInt(Countdowntime / 60.f);
		int32 Seconds = Countdowntime - (Minutes * 60);

		// 시간 두자리수까지 표현
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		ShooterHUD->CharacterOverlay->MatchCountdown->SetText(FText::FromString(CountdownText));

	}
}

// 게임이 끝나면 즉 InProgress 상태에서 WaitNextGame
void AShooterPlayerController::SetHUDPrepareToStartCountDownTime(float PrepareToStartCountDownTime)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

	bool bHUDVaild = ShooterHUD && ShooterHUD->PrepareToAttack && ShooterHUD->PrepareToAttack->TextCountDownTime;


	if (bHUDVaild)
	{
		if (PrepareToStartCountDownTime < 0.f)
		{
			ShooterHUD->PrepareToAttack->TextCountDownTime->SetText(FText());
			return;
		}


		int32 Seconds = PrepareToStartCountDownTime;

		FString CountdownText = FString::Printf(TEXT("%d"), Seconds);
		ShooterHUD->PrepareToAttack->TextCountDownTime->SetText(FText::FromString(CountdownText));

		// 카운트다운 text 애니메이션 구현 필요  나중에 하도록 하자




	}
}

void AShooterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	//int32 SecondsRemaining = 0;

	if (MatchState == MatchState::WaitingToStart) TimeLeft = WatingTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WatingTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::WaitNextGame) TimeLeft = WaitNextGameTime + WatingTime + MatchTime - GetServerTime() + LevelStartingTime;
	
	

	// 반올림 함수 CeilToInt
	// gettimeseconds 함수는 클라이언트가 시작될을때 가는 시간이라서  늦게 켜진 클라일 경우 먼저 켜진 클라와 같은 시간을 가지지 못하는 버그가 있다
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())
	{
		if (ShootingGameMode == nullptr)
		{
			ShootingGameMode = Cast<AShootingGameMode>(UGameplayStatics::GetGameMode(this));
			LevelStartingTime = ShootingGameMode->LevelStartingTime;
		}

		ShootingGameMode = ShootingGameMode == nullptr ? Cast<AShootingGameMode>(UGameplayStatics::GetGameMode(this)) : ShootingGameMode;

		if (ShootingGameMode)
		{
			SecondsLeft = FMath::CeilToInt(ShootingGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDPrepareToStartCountDownTime(TimeLeft);
			
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);   

		}
		else if (MatchState == MatchState::WaitNextGame)
		{
			//SetHUDPrepareToStartCountDownTime(TimeLeft);
			

			

		}

	}

	CountdownInt = SecondsLeft;

}

void AShooterPlayerController::SRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	CReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);

}

void AShooterPlayerController::CReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	// 클라이언트 시간 - 120초 뺀 값을 초기화
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;

	SingleTripTime = 0.5f * RoundTripTime;

	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;

	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();

}

float AShooterPlayerController::GetServerTime()
{
	// 서버라면 true
	if (HasAuthority())
	{
		//return GetWorld()->GetDeltaSeconds();
		return GetWorld()->GetTimeSeconds();  // default return value
	}
	else
	{
		//return GetWorld()->GetDeltaSeconds() + ClientServerDelta;
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

// 서버와 클라의 시간 동기화에 필요한 상속된 함수
void AShooterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		SRequestServerTime(GetWorld()->GetTimeSeconds());
	}

}

void AShooterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandlePrepareToAttackWidget();
		//UE_LOG(LogTemp, Warning, TEXT("Server InProgress is %s"), *State.ToString());

	}
	else if (MatchState == MatchState::WaitNextGame)
	{
		HandleWaitNextGameStateWidget();
		//UE_LOG(LogTemp, Warning, TEXT("Server WaitNextGame is %s"), *State.ToString());
	}
}

void AShooterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandlePrepareToAttackWidget();
		//UE_LOG(LogTemp, Warning, TEXT("Client InProgress is %s"), *MatchState.ToString());

	}
	else if (MatchState == MatchState::WaitNextGame)
	{
		HandleWaitNextGameStateWidget();
		//UE_LOG(LogTemp, Warning, TEXT("Client WaitNextGame is %s"), *MatchState.ToString());
	}
}





void AShooterPlayerController::HandlePrepareToAttackWidget()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		if (ShooterHUD->CharacterOverlay == nullptr) ShooterHUD->AddCharacterOverlay(); 
		
		if (ShooterHUD->PrepareToAttack)
		{
			ShooterHUD->PrepareToAttack->SetVisibility(ESlateVisibility::Hidden);
			
		}
	}
}

void AShooterPlayerController::HandleWaitNextGameStateWidget()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	if (ShooterHUD)
	{
		ShooterHUD->CharacterOverlay->RemoveFromParent();
		
		bool IsValid = ShooterHUD->PrepareToAttack && ShooterHUD->PrepareToAttack->TextPrepareToAttack && ShooterHUD->PrepareToAttack->TextCountDownTime;

		if (IsValid)
		{
			ShooterHUD->PrepareToAttack->SetVisibility(ESlateVisibility::Visible);
			FString PrepareToText = TEXT("새로운 게임 재시작");
			ShooterHUD->PrepareToAttack->TextPrepareToAttack->SetText(FText::FromString(PrepareToText));
			ShooterHUD->PrepareToAttack->TextCountDownTime->SetText(FText::FromString(""));			// test  카운트다운 지우기


			// DOTO : 높은 점수를 가진 유저의 승패를 표시할 기능 구현
			class AShooterGameState* ShooterGameState = Cast<AShooterGameState>(UGameplayStatics::GetGameState(this));
			class AShooterPlayerState* ShooterPlayerState = GetPlayerState<AShooterPlayerState>();
			if (ShooterGameState && ShooterPlayerState)
			{
				TArray<AShooterPlayerState*> PlayerStateArray = ShooterGameState->HighScoringArray;

				FString InfoTextString;
				if (PlayerStateArray.Num() == 0)
				{
					// todo : 게임 끝나고 나오는 위젯에 textblock 을 추가하고 점수와 유저 이름을 출력
					InfoTextString = TEXT("승리자가 없습니다");
				}
				else if (PlayerStateArray.Num() == 1 && PlayerStateArray[0] == ShooterPlayerState)
				{
					InfoTextString = TEXT("승리");
				}
				else if (PlayerStateArray.Num() == 1)
				{
					//InfoTextString = FString::Printf(TEXT("Winner is \n%s"), *PlayerStateArray[0]->GetPlayerName());
					InfoTextString = TEXT("패배");
				}
				else if (PlayerStateArray.Num() > 1)
				{
					InfoTextString = TEXT("무승부");
				}

				ShooterHUD->PrepareToAttack->TextInfo->SetText(FText::FromString(InfoTextString));

			}


		}
	}
}

void AShooterPlayerController::SendMessage(const FText& Text)
{
	class UShootGameInstance* GameInstance = GetGameInstance<UShootGameInstance>();
	class AShooterHUD* HUD = Cast<AShooterHUD>(GetHUD());

	if (GameInstance == nullptr) return;
	if (HUD == nullptr) return;


	if (GameInstance)
	{
		// 유저 이름을 못가져옴 :  test 후 가져오는걸 확인 했음 
		FString UserName = GameInstance->GetUserName();

		//UE_LOG(LogTemp, Warning, TEXT("UserName is (address) : %s"), &UserName);
		//UE_LOG(LogTemp, Warning, TEXT("UserName is (pointer) : %s"), *UserName);

		//GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Blue, FString::Printf(TEXT("T_UserName : %s"), *UserName));

		//test


		FString Message = FString::Printf(TEXT("[%s]: %s"), *UserName, *Text.ToString());



		
		// 임시 text만 받아오는걸로 변경 했고 문제 없음 
		//FString Message = FString::Printf(TEXT("%s"), *Text.ToString());
		
		
		ClientToServerSendMessage(Message); 
	}

}

void AShooterPlayerController::FocusChatInputText()
{
	AShooterHUD* HUD = GetHUD<AShooterHUD>();

	if (HUD == nullptr && ChatSystem == nullptr) return;


	// 입력 할 위젯과 플레이어 컨트롤러의 컨트롤을 변경가능한 구조체
	FInputModeUIOnly InputModeUIOnly;
	//FInputModeGameOnly InputModeGameOnly;

	//enter 키 입력하면 위젯의 포커스를 변경 
	InputModeUIOnly.SetWidgetToFocus(HUD->GetChatInputTextObject()); //default InputModeUIOnly
	
	InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);


	// test
	/*APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	PlayerController->SetShowMouseCursor(false);*/

	SetInputMode(InputModeUIOnly); // 사용중

}

void AShooterPlayerController::FocusGame()
{
	SetInputMode(FInputModeGameOnly());  

	// 다시 돌려 놓기 
	//EnableInput(this);
	//this->SetShowMouseCursor(false);

	
}

void AShooterPlayerController::ClientToServerSendMessage_Implementation(const FString& Message)
{
	// 레벨에 배치된 모든 컨트롤러를 가진 액터(캐릭터)를 가져와 배열에 저장
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetPawn()->GetWorld(), APlayerController::StaticClass(), OutActors);

	// 저장된 배열에 액터 수를 확인 후 클라이언트에 메세지를 보낸다
	for (AActor* actor : OutActors)
	{
		AShooterPlayerController* playercontroller = Cast<AShooterPlayerController>(actor);
		if (playercontroller)
		{
			playercontroller->ServerToClientSendMessage(Message);

		}
	}
}

void AShooterPlayerController::ServerToClientSendMessage_Implementation(const FString& Message)
{
	// 클라에서 온 문자를 서버한테도 메시지를 표시한다 
	AShooterHUD* hud = GetHUD<AShooterHUD>();
	if (hud == nullptr) return;

	hud->AddChatMessage(*Message);  // dereference
	
}

void AShooterPlayerController::Announcement_KillFeed(APlayerState* SuspectUserState, APlayerState* VictimUserState)
{
	ClientAnnouncement_KillFeed(SuspectUserState, VictimUserState);

}

void AShooterPlayerController::ClientAnnouncement_KillFeed_Implementation(APlayerState* SuspectUserState, APlayerState* VictimUserState)
{
	class APlayerState* PS = GetPlayerState<APlayerState>();

	if (PS && SuspectUserState && VictimUserState)
	{
		ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;

		if (ShooterHUD)
		{
			// 공격자가 나 일때
			if (PS == SuspectUserState && VictimUserState != PS)
			{
				ShooterHUD->ShowKillFeedWidget(PS->GetPlayerName(), VictimUserState->GetPlayerName());
				return;
			}
			
			// 피해자가 나 일때
			if (PS == VictimUserState && SuspectUserState != PS)
			{
				ShooterHUD->ShowKillFeedWidget(SuspectUserState->GetPlayerName(), PS->GetPlayerName());
				return;
			}

			// 공격자 와 피해자가 나 일때 
			if (SuspectUserState == VictimUserState && SuspectUserState == PS)
			{
				ShooterHUD->ShowKillFeedWidget(PS->GetPlayerName(), "Kill My Self");
				return;
			}

			// 굳이 필요 없을듯 하다
			/*if (SuspectUserState == VictimUserState && SuspectUserState != PS)
			{
				ShooterHUD->ShowKillFeedWidget(SuspectUserState->GetPlayerName(), "Them Kill Self");
				return;
			}*/

			ShooterHUD->ShowKillFeedWidget(SuspectUserState->GetPlayerName(), VictimUserState->GetPlayerName());

		}
	}
}