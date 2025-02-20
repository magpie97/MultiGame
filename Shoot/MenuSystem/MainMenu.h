// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData 
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};


UCLASS()
class SHOOT_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	// FObjectInitializer 생성자에 사용하는 이유
	// 기본 생성자로 전달(우리는 아무것도 ObjectInitializer를 사용하지 않는다
	// 이것은 Super( ObjectInitializer )를 호출하는 상속된 클래스와의 호환성을 위한 것 (꼭 사용안해도 된다)
	UMainMenu(const FObjectInitializer& ObjectInitializer);

	//랩핑 함수
	UFUNCTION()
	void SetServerList(TArray<FServerData> ServerNames);

	UFUNCTION()
	void SelectIndex(uint32 Index);

protected:
	virtual bool Initialize();


private:
	UPROPERTY()
	TSubclassOf<class UUserWidget> ServerRowClass;

	// 메인 화면 버튼들
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OptionButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* ExitToDesktopButton;


	


	// 메인 메뉴 widget switcher 내부의 버튼들
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* JoinMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* HostMenu;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerHostName;			// 서버이름 입력 받기위한

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateGame_CreateGameButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CreateGame_CancelButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* JoinMenu_CancelButton;







	// 조인 메뉴에 join확인 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinMenu_ConfirmButton;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* JoinMenu_ServerList;

	UPROPERTY(EditAnywhere, Category = "LobbyGameMode")
	class ALobbyGameMode* LobbyGameMode;





private:

	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void ExitToDesktopPressed();

	
	TOptional<uint32> SelectedIndex;

	UFUNCTION()
	void UpdateChildren();
	
};
