// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "InGameMenu.generated.h"

class UButton;

/**
 * 
 */


UCLASS()
class SHOOT_API UInGameMenu : public UMenuWidget
{
	GENERATED_BODY()
protected:
	// 하위 클래스에서 오버라이드 하고 싶지만 외부에서 호출되는 것은 원하지 않을때 사용
	virtual bool Initialize();



private:
	// 인 게임 메뉴 버튼들
	UPROPERTY(meta = (BindWidget))
	UButton* IGM_BackButton;

	UPROPERTY(meta = (BindWidget))
	UButton* IGM_LeaveGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* IGM_OptionButton;

	UPROPERTY(meta = (BindWidget))
	UButton* IGM_ExitGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* IGM_BG_BackButtonSection;




public:
	UFUNCTION()
	void BackPressed();

	UFUNCTION()
	void LeaveGamePressed();
	
};
