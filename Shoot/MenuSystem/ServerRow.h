// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	// 서버 이름
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ServerNames;

	// 호스트 이름
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HostUser;

	// 총 입장 가능한 수
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ConnectionFraction;

	UPROPERTY(BlueprintReadOnly)
	bool Selected = false;

	UFUNCTION()
	void Setup(class UMainMenu* Parent, uint32 Index);


private:
	// 가로로 표현되는 세션의 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* RowButton;

	UPROPERTY()
	class UMainMenu* Parent;

	UPROPERTY()
	uint32 Index;


	UFUNCTION()
	void OnClicked();
	
};
