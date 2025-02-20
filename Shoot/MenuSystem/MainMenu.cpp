// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/PanelWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "ServerRow.h"
#include "Components/TextBlock.h"

#include "Shoot/LobbyGameMode.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
	// 블루프린트에서 불러올 것 임을 명시
	// C:/Users/48652/Documents/Unreal Projects/Shoot/Content/UI/WBP_ServerRow.uasset
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/UI/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	ServerRowClass = ServerRowBPClass.Class;
}



bool UMainMenu::Initialize()
{
	// 부모 클래스 Super 키워드를 사용해서 초기화 해준다
	bool Success = Super::Initialize();
	if (!Success)
	{
		return false;
	}

	// 체크: 에디터를 끄지 않고 계속 실행 할수 있는 메크로인 ensure를 사용한다 
	if (!ensure(HostButton != nullptr)) return false;
	// HostButton을 누르면 델리게이트로 CreateGame 함수를 호출
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(CreateGame_CancelButton != nullptr)) return false;
	// CreateGame_CancelButton을 누르면 델리게이트로 OpenMainMenu 함수를 호출
	CreateGame_CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(CreateGame_CreateGameButton != nullptr)) return false;
	// CreateGame_CreateGameButton을 누르면 델리게이트로 HostServer 함수를 호출
	CreateGame_CreateGameButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

	if (!ensure(JoinButton != nullptr)) return false;
	// JoinButton을 누르면 델리게이트로 JoinGame 함수를 호출
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(ExitToDesktopButton != nullptr)) return false;
	// JoinButton을 누르면 델리게이트로 ExitToDesktopPressed 함수를 호출
	ExitToDesktopButton->OnClicked.AddDynamic(this, &UMainMenu::ExitToDesktopPressed);

	if (!ensure(JoinMenu_CancelButton != nullptr)) return false;
	// JoinButton을 누르면 델리게이트로 JoinGame 함수를 호출
	JoinMenu_CancelButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

	if (!ensure(JoinMenu_ConfirmButton != nullptr)) return false;
	// JoinButton을 누르면 델리게이트로 JoinGame 함수를 호출
	JoinMenu_ConfirmButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);


	return true;
}

void UMainMenu::OpenHostMenu()
{
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::HostServer()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("HostServer is Successful"));
	}

	if (MenuInterface != nullptr)
	{
		FString ServerName = ServerHostName->Text.ToString();
		MenuInterface->Host(ServerName);
	}
}

void UMainMenu::SetServerList(TArray<FServerData> ServerNames)
{
	UWorld* World = this->GetWorld();
	if (!ensure(World != nullptr)) return;

	JoinMenu_ServerList->ClearChildren();

	uint32 i = 0;
	for (const FServerData& ServerData : ServerNames)		// const 잊어먹지 말기 (주석처리해서 테스팅)
	{
		UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);
		if (!ensure(Row != nullptr)) return;

		Row->ServerNames->SetText(FText::FromString(ServerData.Name));
		Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));

		//testing------------------------------------------------------------		%%% 안됨 빌드는 되는데 
		//UE_LOG(LogTemp, Warning, TEXT("LobbyGameMode is null"));
		//ServerData.CurrentPlayers = LobbyGameMode->GetNumberOfPlayers();		// 이거 빼고 다른건 테스트
		//UE_LOG(LogTemp, Warning, TEXT("update CurrentPlayer : %d"), ServerData.CurrentPlayers);

		//testing------------------------------------------------------------
		
		// 서버 총 인원 표시
		FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);

		Row->ConnectionFraction->SetText(FText::FromString(FractionText));

		Row->Setup(this, i);
		++i;

		JoinMenu_ServerList->AddChild(Row);

	}
}

void UMainMenu::SelectIndex(uint32 Index)
{
	//set
	// 선택적 값이 있을 때 IsSet()은 true를 반환하고 GetValue()는 의미가 있다
	// *그렇지 않으면 GetValue()는 의미가 없다
	SelectedIndex = Index;
	UpdateChildren();

}

void UMainMenu::UpdateChildren()
{
	// GetChildrenCount 는 컨테이너의 하위 위젯 수를 가져온다
	for (int32 i = 0; i < JoinMenu_ServerList->GetChildrenCount(); i++)
	{
		auto Row = Cast<UServerRow>(JoinMenu_ServerList->GetChildAt(i));
		if (Row != nullptr)
		{
			Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
		}

	}
}

void UMainMenu::JoinServer()
{
	// bIsSet은 해당 데이터의 생성자가 호출되었는지 여부를 판단하는 기능이다
	// Value는 실제 넘기는 데이터
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)		// xxx  isSet 및 MenuInterface가 null임  문제 해결해야함
	{

		UE_LOG(LogTemp, Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected index not set"));

	}


}

void UMainMenu::OpenJoinMenu()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("OpenJoinMenu is Successful"));
	}

	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(JoinMenu != nullptr)) return;
	
	// switcher 내부에 있는 joinmenu 를 표시
	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (MenuInterface != nullptr)
	{
		MenuInterface->RefreshServerList();
	}
	
}

void UMainMenu::OpenMainMenu()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("OpenMainMenu is Successful"));
	}

	if (!ensure(MenuSwitcher != nullptr)) return;
	if (!ensure(MainMenu != nullptr)) return;

	// switcher 내부에 있는 MainMenu 를 표시  : 뒤로가기를 지우지 않고 구현
	MenuSwitcher->SetActiveWidget(MainMenu);

}

void UMainMenu::ExitToDesktopPressed()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	// 콘솔 커맨드 접근
	PlayerController->ConsoleCommand("quit");

	

}
