// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatSystem.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UChatSystem : public UUserWidget
{
	GENERATED_BODY()

private:

	UPROPERTY(meta = (BindWidget))
	class UChat* WBP_Chat;




public:
	TSharedPtr<class SWidget> GetChatInputTextObject();
	
	UFUNCTION()
	void AddChatMessage(const FString& message);


	
};
