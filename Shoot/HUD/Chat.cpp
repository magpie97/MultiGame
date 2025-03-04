// Fill out your copyright notice in the Description page of Project Settings.


#include "Chat.h"
#include "Components/TextBlock.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/EditableText.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/GameState/ShooterGameState.h"
#include "Fonts/SlateFontInfo.h"
#include "GameFramework/PlayerController.h"
#include "Shoot/HUD/ChatSystem.h"


void UChat::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본 폰트를 가져오는 파인더
	//static ConstructorHelpers::FObjectFinder<UFont> RobotoFontObj(*UWidget::GetDefaultFontName());

	ChatInput->OnTextCommitted.AddDynamic(this, &UChat::OnChatTextCommitted);


}

void UChat::AddChatMessage(const FString& chat)
{
	class UTextBlock* NewText = NewObject<UTextBlock>(ChatHistory);

	//SetConsoleOutputCP(65001);
	//setlocale(LC_ALL, "ko_KR.UTF-8");




	// 기본 폰트를 가져와서 처리   폰트스타일을 안가져와서 강제로 초기화
	FSlateFontInfo SlateFontInfo = FCoreStyle::Get().GetFontStyle("EmbossedText");
	// 폰트 사이즈 
	SlateFontInfo.Size = 12;
	// 폰트 사이즈 및 설정 초기화
	NewText->SetFont(SlateFontInfo);
	NewText->SetText(FText::FromString(*chat));


	// newtext를 chatsystem에 자식 클래스로 넣기
	ChatHistory->AddChild(NewText);

	// 채팅의 맨 끝으로 
	ChatHistory->ScrollToEnd();



	/*
		test 메모리 해제  
	*/
	NewText->ConditionalBeginDestroy();
	NewText = nullptr;


}

void UChat::SetChatInputTextMessage(const FText& Text)
{
	ChatInput->SetText(Text);
}

TSharedPtr<class SWidget> UChat::GetChatInputTextObject()
{
	return ChatInput->GetCachedWidget();
}

void UChat::OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	// game state 를 가져와서 포커스 변경후 empty 함수 실행 하기전  입력 안되게 하기
	UWorld* World = GetWorld();
	if (World == nullptr) return;

	class AShooterPlayerController* PlayerController = Cast<AShooterPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PlayerController == nullptr) return;

	class AShooterGameState* GameState = GetWorld() != nullptr ? World->GetGameState<AShooterGameState>() : nullptr;

	FInputModeUIOnly InputModeUIOnly;
	FInputModeGameAndUI InputModeGameAndUI;

	FName GameStateWaitingToStart = "WaitingToStart";
	FName GameStateInProgress = "InProgress";



	switch (CommitMethod)
	{
	case ETextCommit::OnEnter:
		if (Text.IsEmpty() == false)
		{
			

			if (GameState->GetMatchState() == GameStateWaitingToStart)
			{
				// watingtostart 가 아니라면  입력 안되게 변경
				SetChatInputTextMessage(Text.GetEmpty());

				PlayerController->FocusGame();
				break;
			
			}
			else if(GameState->GetMatchState() == GameStateInProgress)
			{
				// ===========   test
				//this->bIsFocusable = true; // test
				////InputModeData.SetWidgetToFocus(this->TakeWidget());
				//InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockOnCapture);
				//PlayerController->SetInputMode(InputModeData);
				//PlayerController->bShowMouseCursor = false;
				//============    test

				//UGameplayStatics::SetViewportMouseCaptureMode(GetWorld()->GetFirstPlayerController(), EMouseCaptureMode::CaptureDuringMouseDown);
				//UGameplayStatics::SetViewportMouseCaptureMode(GetWorld()->GetFirstPlayerController(), EMouseCaptureMode::NoCapture);
				//UGameplayStatics::SetViewportMouseCaptureMode(GetWorld()->GetFirstPlayerController(), EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown);
				
				//PlayerController->SetIgnoreLookInput(true);
				//PlayerController->bShowMouseCursor = false;


				//InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
				////InputModeData.SetWidgetToFocus(this->TakeWidget());
				//PlayerController->SetInputMode(InputModeData);
				//PlayerController->SetShowMouseCursor(false);
				//bIsFocusable = true;

				//PlayerController->SetInputMode(FInputModeGameAndUI());



				//InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
				//InputModeGameAndUI.SetHideCursorDuringCapture(true);

				//PlayerController->DisableInput(PlayerController);
				//PlayerController->SetInputMode(InputModeGameAndUI);
				//PlayerController->SetShowMouseCursor(false);
				bIsFocusable = true;

				//InputModeUIOnly.SetLockMouseToViewportBehavior(EMouseCursor::);

				
				


				PlayerController->SendMessage(Text);
				SetChatInputTextMessage(Text.GetEmpty());

			}
			//test
			//PlayerController->ResetIgnoreLookInput();
			
		}
		


		PlayerController->FocusGame();
		//InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);  //test
		break;
	
	case ETextCommit::OnCleared:
		PlayerController->FocusGame();
		break;

	}
}
