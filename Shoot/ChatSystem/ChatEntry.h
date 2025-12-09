// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatEntry.generated.h"

/**
 * 
 */
class UTextBlock;


UCLASS()
class SHOOT_API UChatEntry : public UUserWidget
{
	GENERATED_BODY()


protected:
	virtual void NativeConstruct() override;


public:

	// 	UPROPERTY(meta = (BindWidget))

	UPROPERTY()
	UTextBlock* TextMessage_TextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat", meta = (ExposeOnSpawn = "true"))
	FString TextMessage;

	UFUNCTION()
	void SetText();
	
};
