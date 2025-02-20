// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Chat.generated.h"

/**
 * 
 */
UCLASS()
	class SHOOT_API UChat : public UUserWidget
{
	GENERATED_BODY()

public:
	// addviewport 를 실행시키면 실행되는 함수라는데 한번 써본다
	virtual void NativeConstruct() override;

	UFUNCTION()
	void AddChatMessage(const FString& chat);

	UFUNCTION()
	void SetChatInputTextMessage(const FText& Text);

	TSharedPtr<class SWidget> GetChatInputTextObject();




public:
	UPROPERTY(meta = (BindWidget))
	class UScrollBox* ChatHistory;

	UPROPERTY(meta = (BindWidget))
	class UEditableText* ChatInput;


	
	UFUNCTION()
	void OnChatTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);




	
};
