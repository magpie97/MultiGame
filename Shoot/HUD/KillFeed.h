// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeed.generated.h"

/**
 * 
 */

class UBackgroundBlur;
class UHorizontalBox;
class UTextBlock;
class UImage;

//USTRUCT(BlueprintType)
//struct DieWhatForImageStruct
//{
//	GENERATED_BODY()
//
//	UPROPERTY()
//	UImage* WeaponImage;
//
//	UPROPERTY()
//	UImage* GrenadeImage;
//
//	UPROPERTY()
//	UImage* KillMySelfImage;
//
//};



UCLASS()
class SHOOT_API UKillFeed : public UUserWidget
{
	GENERATED_BODY()

public:


	UPROPERTY()
	UBackgroundBlur* BG_Blur;

	UPROPERTY()
	UHorizontalBox* KillFeed_HorizontalBox;

	UPROPERTY()
	UTextBlock* SuspectUserName_TextBlock;

	UPROPERTY()
	UTextBlock* VictimUserName_TextBlock;

	UPROPERTY()
	UImage* DieWhatFor_Image;




	
	// Animations
	UPROPERTY(BlueprintReadOnly, Transient, Category = "KillFeed", meta = (BindWidgetAnim))
	class UWidgetAnimation* ShowKillFeedAnimation;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "KillFeed", meta = (BindWidgetAnim))
	class UWidgetAnimation* DisappearKillFeedAnimation;


	
	
public:

	UFUNCTION()
	void ShowKillFeed(FString SuspectName/*, UImage* Image*/, FString VictimName);

	UFUNCTION()
	void ShowKillFeedWidgetAnimation();

	UFUNCTION()
	void DisappearKillFeedWidgetAnimation();
	
};
