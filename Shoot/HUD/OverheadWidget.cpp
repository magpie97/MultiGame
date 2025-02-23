// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/Character/BaseCharacter.h"






void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
	/*ENetRole RemoteRole = InPawn->GetRemoteRole();
	FString Role;
	switch (RemoteRole)
	{
	case ENetRole::ROLE_Authority:
		Role = FString("Authority");
		break;
	case ENetRole::ROLE_AutonomousProxy:
		Role = FString("Autonomous Proxy");
		break;
	case ENetRole::ROLE_SimulatedProxy:
		Role = FString("Simulated Proxy");
		break;
	case ENetRole::ROLE_None:
		Role = FString("None");
		break;
	}*/
	//FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);

	// testing code
	/*const APlayerState* PlayerState = InPawn->GetPlayerState();
	if (!PlayerState || !*PlayerState->GetPlayerName() && TotalTime < GetPlayerNameTimeout)
	{
		FTimerHandle GetPlayerStateTimer;
		FTimerDelegate TryAgainDelegate;
		TryAgainDelegate.BindUFunction(this, FName("ShowPlayerName"), InPawn);
		GetWorld()->GetTimerManager().SetTimer(GetPlayerStateTimer, TryAgainDelegate, GetPlayerNameInterval, false, 0.1f);
		TotalTime += GetPlayerNameInterval;
		return;
	}
	FString PlayerName = InPawn->GetPlayerState()->GetPlayerName();
	SetDisplayText(PlayerName);
	*/


	/*APlayerState* PlayerState = InPawn->GetPlayerState();
	if (PlayerState)
	{
		FString PlayerName = FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName());
		SetDisplayText(PlayerName);


	}*/

	//SetDisplayText(PlayerName);
	
	/*FString PlayerName = InPawn->GetPlayerState()->GetPlayerName();
	SetDisplayText(PlayerName);*/


	



}

//void UOverheadWidget::ShowUserNameWidget(APawn* InPawn)
//{
//	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//
//	PlayerState = Cast<APlayerController>(PlayerController->PlayerState);
//	
//	FString UserName = FString::Printf(TEXT("%s"), *PlayerState->GetPlayerName());
//
//	// debug
//	GEngine->AddOnScreenDebugMessage(5, 15.f, FColor::Red, *UserName);
//
//	SetDisplayText(UserName);
//
//
//}

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	RemoveFromParent();
	Super::OnLevelRemovedFromWorld(InLevel, InWorld);
}