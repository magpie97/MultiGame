// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "ShooterGameState.generated.h"



class AShooterPlayerState;

UCLASS()
class SHOOT_API AShooterGameState : public AGameState
{
	GENERATED_BODY()

public:

	// 복사
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	// 높은 킬 스코어 유저 정보 업데이트
	void UpdateHighScore(AShooterPlayerState* PlayerHighScore);

	// 킬 스코어 높은 유저의 점수 저장
	UPROPERTY(Replicated)
	TArray<AShooterPlayerState*> HighScoringArray;

private:

	UPROPERTY()
	float HighScore = 0.f;

	
	
};
