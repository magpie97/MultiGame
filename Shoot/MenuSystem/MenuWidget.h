// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// viewport 메뉴 위젯 표시
	UFUNCTION()
	void Setup();

	// viewport 메뉴 위젯 지우기
	UFUNCTION()
	void Teardown();

	// setter 함수
	void SetMenuInterface(IMenuInterface* _MenuInterface);

	

protected:
	IMenuInterface* MenuInterface;
};
