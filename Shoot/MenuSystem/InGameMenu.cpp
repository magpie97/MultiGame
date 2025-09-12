// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"


bool UInGameMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	// 바인딩
	if (!ensure(IGM_BackButton != nullptr)) return false;
	IGM_BackButton->OnClicked.AddDynamic(this, &UInGameMenu::BackPressed);

	// 바인딩
	if (!ensure(IGM_LeaveGameButton != nullptr)) return false;
	IGM_LeaveGameButton->OnClicked.AddDynamic(this, &UInGameMenu::LeaveGamePressed);

	// test code
	if (!ensure(IGM_BG_BackButtonSection != nullptr)) return false;
	IGM_BG_BackButtonSection->OnClicked.AddDynamic(this, &UInGameMenu::BackPressed);

	return true;
}

void UInGameMenu::BackPressed()
{
	Teardown();
}

void UInGameMenu::LeaveGamePressed()
{
	// MenuInterface.h 에 선언된 MenuInterface 변수
	// MenuWidget을 상속받고 있는 클래스 
	if (MenuInterface != nullptr) {
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}