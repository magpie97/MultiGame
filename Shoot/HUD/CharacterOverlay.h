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
	UPROPERTY(VisibleAnywhere, Category = "Health", meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(VisibleAnywhere, Category = "Health", meta = (BindWidget))
	class UTextBlock* HealthText;

	// 케릭터가 죽였을때 
	UPROPERTY(VisibleAnywhere, Category = "PlayerScore", meta = (BindWidget))
	class UTextBlock* PlayerKillScore;

	// 케릭터가 죽었을때 
	UPROPERTY(VisibleAnywhere, Category = "PlayerScore", meta = (BindWidget))
	UTextBlock* PlayerDeathScore;

	// 무기에 대한 총알 기본 탄창
	UPROPERTY(VisibleAnywhere, Category = "Ammo", meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdown;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadeAmount;




	// warnning animations
	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* NetworkProblem;

	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* LowFPS;

	//UPROPERTY(meta = (BindWidget), Transient)
	//class UWidgetAnimation* NetworkProblemAnimation;

	//UPROPERTY(meta = (BindWidget), Transient)
	//class UWidgetAnimation* LowFPSAnimation;





private:


	
};