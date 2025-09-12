// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KillFeed.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UKillFeed : public UUserWidget
{
	GENERATED_BODY()


public:

	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* HorizontalBox_KillFeed;

	UPROPERTY(meta = (BindWidget))
	class UTextBox* TextBox_KillFeed;







	
};
