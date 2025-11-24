// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PrepareToAttack.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UPrepareToAttack : public UUserWidget
{
	GENERATED_BODY()

protected:

	//virtual void NativeConstruct() override;

public:
	//void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:

	//TSubclassOf<UUserWidget> PrepareToAttackWidget;

	// text to 전투 시작 준비
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextPrepareToAttack;

	// text to countdowntime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextCountDownTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* TextInfo;

	UPROPERTY()
	float time = 5.f;

	

	/*UPROPERTY()
	class AShootingGameMode* ShootingGameMode;

	UFUNCTION()
	void CountDownTimePlaySound();*/
	
};
