// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeed.generated.h"

/**
 * 
 */

//class UTextBlock;
//class UImage;


UCLASS()
class SHOOT_API UKillFeed : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class UBackgroundBlur* BG_Blur;

	UPROPERTY()
	class UHorizontalBox* KillFeed_HorizontalBox;

	UPROPERTY()
	class UTextBlock* SuspectUserName_TextBlock;

	UPROPERTY()
	class UTextBlock* VictimUserName_TextBlock;
	
	// Animations
	UPROPERTY(BlueprintReadOnly, Transient, Category = "KillFeed", meta = (BindWidgetAnim))
	class UWidgetAnimation* ShowKillFeedAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "KillFeed", meta = (BindWidgetAnim))
	class UWidgetAnimation* DisappearKillFeedAnimation;


	
	
public:

	UFUNCTION()
	void ShowKillFeed(FString SuspectName, FString VictimName);

	UFUNCTION()
	void ShowKillFeedWidgetAnimation();

	UFUNCTION()
	void DisappearKillFeedWidgetAnimation();
	
};
