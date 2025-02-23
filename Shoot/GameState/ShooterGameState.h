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

	// ����
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	// ���� ų ���ھ� ���� ���� ������Ʈ
	void UpdateHighScore(AShooterPlayerState* PlayerHighScore);

	// ų ���ھ� ���� ������ ���� ����
	UPROPERTY(Replicated)
	TArray<AShooterPlayerState*> HighScoringArray;

private:

	UPROPERTY()
	float HighScore = 0.f;

	
	
};
