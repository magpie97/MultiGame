// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"


void UMenuWidget::Setup()
{
	// 뷰 포트에 widget 표시
	this->AddToViewport(); // test this
	//UserWidget->AddToViewport();


	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	FInputModeUIOnly InputModeData;
	this->bIsFocusable = true; // test
	//InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;


}


void UMenuWidget::Teardown()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	// UUserWidget::OnLevelRemovedFromWorld() 는 레벨이 기본적으로 종료될때 호출되는 함수  // 메뉴 레벨을 떠날 때 자동 호출됨
	//this->RemoveFromViewport();  // 안되면 this로 처리
	this->RemoveFromViewport();

	// FInputModeDataBase 안에  GameOnly, UIOnly, GameAndUI 등 정의되어 있음 
	// GameAndUI 는 폰의 움직임과 UI를 동시에 표현가능할꺼 같다

	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);

	PlayerController->bShowMouseCursor = false;

}

void UMenuWidget::SetMenuInterface(IMenuInterface* _MenuInterface)
{
	/*
	this는 MenuInterface라는 이름이 모호하기 때문에 사용한다
	클래스 프로퍼티인지 또는 전달되는 인자인지 알 수 없기 때문
	:: 오류 발생  이름을 다르게 지어주자
	*/
	this->MenuInterface = _MenuInterface;
}