// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerScoreRawInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UPlayerScoreRawInfoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

public:
	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	class UTextblock* PlayerName;								   
																   
	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	class UTextblock* PlayerScore;								   
																   
	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	class UTextblock* PlayerKill;								   
																   
	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	class UTextblock* PlayerDie;



private:
	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	FString Name;

	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	uint16 Score;

	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	uint16 Kill;

	UPROPERTY(VisibleAnywhere, Category = "PlayerScoreInfo", meta = (BindWidget))
	uint16 Die;

	void SetPlayerInfo();

	
};
