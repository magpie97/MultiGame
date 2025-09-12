// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	// 케릭터가 죽였을때 
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerKillScore;

	// 케릭터가 죽었을때 
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerDeathScore;

	// 무기에 대한 총알 기본 탄창
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdown;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadeAmount;


private:


	
};