// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootGameInstance.h"
#include "ShootCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "MenuSystem/MainMenu.h"
#include "Menusystem/MenuWidget.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GameFramework/PlayerState.h"
#include "Shoot/ShooterPlayerState/ShooterPlayerState.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/GameState/ShooterGameState.h"



// 전역 변수 선언  
const static FName SESSION_NAME = TEXT("GameSession");
const static FName SERVER_NAME_SETTING_KEY = TEXT("ServerName");


// 오류 날경우 MainMenu->bIsFocusable = true; 추가
UShootGameInstance::UShootGameInstance(const FObjectInitializer& ObjectInitializer)
{
	// class 찾기
	// C:/Users/48652/Documents/Unreal Projects/Shoot/Content/ThirdPersonCPP/Blueprints/ThirdPersonCharacter.uasset
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/UI/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;

	MenuClass = MenuBPClass.Class;

	// C:/Users/48652/Documents/Unreal Projects/Shoot/Content/UI/WBP_InGameMenu.uasset
	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/UI/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;

	InGameMenuClass = InGameMenuBPClass.Class;

	
	

}

// 
void UShootGameInstance::Init()
{
	//주어진 서비스에 대한 온라인 하위 시스템 가져오기
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();

	// !ensure() 지움   지워도 잘됨 
	if (OnlineSubsystem != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("OnlineSubsystem Init() "));


		//UE_LOG(LogTemp, Warning, TEXT("Found Subsystem : %s"), *OnlineSubsystem->GetSubsystemName().ToString());
		// 세션 관리 서비스에 액세스하기 위한 인터페이스 가져오기
		SessionInterface =  OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Found SessionInterface"));

			/*
			콜백
			UObject 기반 멤버 함수 델리게이트를 추가합니다. 
			UObject 델리게이트는 자신의 오브젝트에 대한 약 레퍼런스를 유지합니다.
			*/
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UShootGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UShootGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UShootGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UShootGameInstance::OnJoinSessionComplete);



		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Not Found Subsystem !! is NULL"));
	}

	if (GEngine != nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UShootGameInstance::OnNetworkFailure);
	}
}

// 메인 메뉴 로드
void UShootGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;
	
	// 위젯 만들기
	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;
	

	Menu->Setup();
	// MainMenu. 현재 GameInstance인 'this'로 스스로 설정한다
	Menu->SetMenuInterface(this);



}

void UShootGameInstance::InGameLoadMenuWidget()
{
	if (!ensure(InGameMenuClass != nullptr)) return;

	UMenuWidget* MenuWidget = CreateWidget<UMenuWidget>(this, InGameMenuClass);

	if (!ensure(MenuWidget != nullptr)) return;

	//Ingamemenu 오픈
	MenuWidget->Setup();

	MenuWidget->SetMenuInterface(this);

}

// 게임 만들기 버튼 기능
void UShootGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid())
	{
		// 호스트 버튼을 눌렀을때 동일한 세션이 이미 존재하는지 여부를 먼저 체크 
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			// 존재 하는 경우 destroy 한다
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			// 없다면 세션 만들기 함수 호출
			CreateSession();
		}
	}
}

FString UShootGameInstance::GetUserName()
{
	// first test
	/*PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	PlayerState = Cast<APlayerController>(PlayerController->PlayerState);
	
	FString UserName = FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName());*/

	//==================

	
	// new test
	class AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	auto PlayerState = PlayerController->GetPlayerState<APlayerController>();


	return PlayerController->PlayerState->GetPlayerName();;

}

// 세션이 잘 만들어졌다면 
void UShootGameInstance::OnCreateSessionComplete(FName SessionName, bool Successful)
{
	if (!Successful)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
		return;
	}

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("Hosting"));
	}

	// ServerTravel을 만들기위해 world를 null 체크 한다
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	// C:/Users/48652/Documents/Unreal Projects/Shoot/Content/ThirdPersonCPP/Maps/Lobby.umap
	// World'/Game/ThirdPersonCPP/Maps/Lobby.Lobby'
	World->ServerTravel("/Game/Maps/Lobby?listen");

}

void UShootGameInstance::OnDestroySessionComplete(FName SessionName, bool Successful)
{
	if (Successful)
	{
		// Host버튼에 DestroySession 함수가 실행되면 세션이 파괴 돼고
		// 세션이 파괴가 완료 되었다는 콜백을 받는다   그러면 다시 세션을 만드는 함수 실행
		//UE_LOG(LogTemp, Warning, TEXT("destroyed session"));
		CreateSession();
		
	}
}

void UShootGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	//UE_LOG(LogTemp, Warning, TEXT("destroyed session"));
	LoadMainMenu();

}



void UShootGameInstance::RefreshServerList()
{
	//=========================================================================
// makeShareable 메서드는 표준 C++ 포인터를 가져와 언리얼용 참조 카운트 포인터로 변환한다
// 따라서 MakeShareable 래퍼를 사용하면 오류는 없을 것 스마트포인터여서 new를 쓴다
// FOnlineSessionSearch 는 함수이다
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	/*
	TSharedPtr로 FindSession함수를 사용 할려 했으나 오류
	TSharedPtr를 TSharedRef로 인수를 변환 할 수 없다고 나옴
	해결 방법은 Ptr를 Ref로 변환 시켜주면 된다  .ToSharedRef()
	*/
	if (SessionSearch.IsValid())
	{
		// LAN 인지 
		//SessionSearch->bIsLanQuery = true;  default value 100
		SessionSearch->MaxSearchResults = 100;

		// 이거 찾기 너무 힘듬... FOnlineSessionSteam에 사용되고 있는 매개변수들임 
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		//UE_LOG(LogTemp, Warning, TEXT("Start Find Session"));
		// 인수 1번째 SearchingPlayerNum
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}


void UShootGameInstance::OnFindSessionsComplete(bool Successful)
{
	if (Successful && SessionSearch.IsValid() && Menu != nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));

		TArray<FServerData> ServerNames;

		// auto 로 타입을 확인    TArray 변수 SearchResults 이다
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Find Session name : %s"), *SearchResult.GetSessionIdStr());
			FServerData Data;	
			

			// 공개적으로 사용 가능한 연결 수
			Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections; 

			// 공개적으로 사용 가능한 연결 수(읽기 전용)
			Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections; 

			// 세션의 소유자 이름
			Data.HostUsername = SearchResult.Session.OwningUserName;

			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTING_KEY, ServerName))
			{
				// 주어진 세션 검색 결과에 대한 세션 ID를 반환한다
				Data.Name = ServerName;
			}
			else
			{
				// 주어진 세션 검색 결과에 대한 세션 ID를 반환한다
				Data.Name = "Could not find name.";
			}

			ServerNames.Add(Data);
			
		}
		Menu->SetServerList(ServerNames);
	}
}

void UShootGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;

		// 스팀 로그인의 디버깅이 불가피 할때 사용
		// 로컬 네트워크 상의 멀티플레이어 게임인지 설정한다 (기본은 bIsLanMatch=0)
		if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Blue, TEXT("OnlineSubsystem : LAN On and Steam Offline"));
			SessionSettings.bIsLANMatch = true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 15.f, FColor::Blue, TEXT("OnlineSubsystem : LAN Off  and Steam Online"));
			SessionSettings.bIsLANMatch = false;
		}

		SessionSettings.NumPublicConnections = NumPublicConnections;	// 세션에 참가할 최대 인원 default : NumPublicConnections
		SessionSettings.bShouldAdvertise = true;	// 생성된 세션을 보기위해 
		SessionSettings.bUsesPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;	

		// 전역변수로 대입하였음 Key는 fstring임
		SessionSettings.Set(SERVER_NAME_SETTING_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		


		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);

	}
	
}

// 게임 찾기 버튼 기능
void UShootGameInstance::Join(uint32 Index)
{
	// OnJoinSessionComplete
// TODO : null 인터페이스가 없는지 확인하는것이 필요  그래서 좀더 일찍 리턴할 수 있도록 한다

	// not 연산자를 쓸때 오류가 생긴다면 IsValid를 써보도록 하자
	if (!SessionInterface.IsValid()) return;
	if (!SessionSearch.IsValid()) return;

	if (Menu != nullptr)
	{
		Menu->Teardown();
	}

	// SearchResults /** 주어진 기준을 검색할 때 찾은 모든 세션의 배열 */
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);

	// FOnJoinSessionComplete 는 두개의 매개변수를 사용 FName, 열거형인 EOnJoinSessionCompleteResult 

}

void UShootGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(SessionName, Address))	// SessionName
	{
		//UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine != nullptr)) return;

	Engine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);

	// IOnlineSession::GetResolvedConnectString bool 타입이고 



}

void UShootGameInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	// /Game/ThirdPersonCPP/Maps/ThirdPersonExampleMap
	// World'/Game/Maps/Game.Game'
	PlayerController->ClientTravel("/Game/Maps/Game", ETravelType::TRAVEL_Absolute);

}

