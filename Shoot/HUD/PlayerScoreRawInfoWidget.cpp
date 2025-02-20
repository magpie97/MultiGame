// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerScoreRawInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/PanelWidget.h"


bool UPlayerScoreRawInfoWidget::Initialize()
{
	Super::Initialize();

	if (PlayerName == nullptr) return false;
	if (PlayerScore == nullptr) return false;
	if (PlayerKill == nullptr) return false;
	if (PlayerDie == nullptr) return false;

	return true;
}

void UPlayerScoreRawInfoWidget::SetPlayerInfo()
{
	

}
