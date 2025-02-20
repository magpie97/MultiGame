// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerScoreWidget.generated.h"


class UTextBlock;

USTRUCT()
struct FPlayerInfo
{
	GENERATED_BODY()

	FString		PlayerName;
	uint16		PlayerScore;
	uint16		PlayerKill;
	uint16		PlayerDie;
};


UCLASS()
class SHOOT_API UPlayerScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Score", meta = (BindWidget))
	class UTextBlock* BlueTeam;

	UPROPERTY(VisibleAnywhere, Category = "Score", meta = (BindWidget))
	class UTextBlock* RedTeam;

	UPROPERTY(VisibleAnywhere, Category = "Score", meta = (BindWidget))
	class UTextBlock* BlueTeamScore;

	UPROPERTY(VisibleAnywhere, Category = "Score", meta = (BindWidget))
	class UTextBlock* RedTeamScore;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* BlueTeamPlayerList;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* RedTeamPlayerList;
	
	TSubclassOf<UUserWidget> PlayerInfo;




private:
	void SetWidgetKillScore(float score);





private:
	void SetPlayerInfo(TArray<FPlayerInfo> playerinfo);



	
};
