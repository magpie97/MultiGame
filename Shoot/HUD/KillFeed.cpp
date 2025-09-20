// Fill out your copyright notice in the Description page of Project Settings.


#include "KillFeed.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/HorizontalBox.h"
#include "Components/BackgroundBlur.h"
#include "Animation/WidgetAnimation.h"



void UKillFeed::ShowKillFeed(FString SuspectName, FString VictimName)
{
	FString SuspectUserName = FString::Printf(TEXT("%s"), *SuspectName);
	FString VictimUserName = FString::Printf(TEXT("%s"), *VictimName);

	bool IsValid = KillFeed_HorizontalBox && SuspectUserName_TextBlock && VictimUserName_TextBlock && BG_Blur/* && ShowKillFeedAnimation && DisappearKillFeedAnimation*/;

	if (IsValid)
	{
		SuspectUserName_TextBlock->SetText(FText::FromString(SuspectUserName));
		VictimUserName_TextBlock->SetText(FText::FromString(VictimUserName));

	}
}

void UKillFeed::ShowKillFeedWidgetAnimation()
{
	PlayAnimation(ShowKillFeedAnimation);

}

void UKillFeed::DisappearKillFeedWidgetAnimation()
{
	PlayAnimation(DisappearKillFeedAnimation);

}