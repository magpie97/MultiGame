// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatBox.h"
#include "Components/ScrollBox.h"
#include "Components/EditableText.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/ChatSystem/ChatPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Shoot/ChatSystem/ChatEntry.h"



void UChatBox::NativeConstruct()
{
	class APlayerController* PlayerController = GetOwningPlayer();
	class AShooterPlayerController* ShootingPlayerController = Cast<AShooterPlayerController>(PlayerController);

	//

	if (ChatPlayerControllerClass && ShootingPlayerController)
	{
		ShootingPlayerController->GetComponentByClass(ChatPlayerControllerClass);

		//ChatPlayerControllerRef

	}


}

void UChatBox::FocusChat()
{
	class APlayerController* PlayerController = GetOwningPlayer();
	class AShooterPlayerController* ShootingPlayerController = Cast<AShooterPlayerController>(PlayerController);

	if (ShootingPlayerController)
	{
		InputText_EditableText->SetUserFocus(ShootingPlayerController);

		InputText_EditableText->SetIsEnabled(true);

		ChatFocus = true;

	}
}

void UChatBox::ExitChat()
{
	class APlayerController* PlayerController = GetOwningPlayer();
	class AShooterPlayerController* ShootingPlayerController = Cast<AShooterPlayerController>(PlayerController);

	if (ShootingPlayerController)
	{
		ChatFocus = false;

		InputText_EditableText->SetText(FText::FromString(""));

		InputText_EditableText->SetIsEnabled(false);

		FInputModeGameOnly InputMode;
		ShootingPlayerController->SetInputMode(InputMode);

	}
	

}

void UChatBox::Scroll(bool bUpScroll)
{
	float LimitScrollStrenthValue = UKismetMathLibrary::SelectFloat(-1.f, 1.f, bUpScroll);

	float MultiplierValue = UKismetMathLibrary::Multiply_FloatFloat(ScrollStrength, LimitScrollStrenthValue);

	float ScrollUpOffset = ChatHistory_ScrollBox->GetScrollOffset();

	float Add = UKismetMathLibrary::Add_FloatFloat(MultiplierValue, ScrollUpOffset);

	float ScrollDownOffset = ChatHistory_ScrollBox->GetScrollOffsetOfEnd();

	float f =  FMath::Clamp(Add, 0.f, ScrollDownOffset);

	ChatHistory_ScrollBox->SetScrollOffset(f);


}

void UChatBox::AddChatMessage(FString message)
{
	class APlayerController* PlayerController = GetOwningPlayer();
	class AShooterPlayerController* ShootingPlayerController = Cast<AShooterPlayerController>(PlayerController);

	// 위젯 생성 필요
	if (ChatEntryClass && ChatEntry)
	{
		// 블프엔 string 매개변수 쓸 수 있는데 코드로는 어떻게 해야할지 모르겠네
		ChatEntry = CreateWidget<UChatEntry>(ShootingPlayerController, ChatEntryClass);
		ChatEntry->AddToViewport();

	}


}