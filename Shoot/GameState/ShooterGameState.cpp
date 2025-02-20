// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameState.h"
#include "Shoot/ShooterPlayerState/ShooterPlayerState.h"
#include "Net/UnrealNetwork.h"

void AShooterGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AShooterGameState, HighScoringArray);

}

void AShooterGameState::UpdateHighScore(AShooterPlayerState* PlayerHighScore)
{
	if (HighScoringArray.Num() == 0)
	{
		// ���� ������ ���� ���� ������ add �Լ��� ���� (Emplace �Լ��� ������)
		HighScoringArray.Add(PlayerHighScore);
		HighScore = PlayerHighScore->GetScore();

	}
	else if (PlayerHighScore->GetScore() == HighScore)
	{
		// ���� �����̳ʿ� ������ ������Ʈ�� �̹� �������� �ʴ� ��� �� ������Ʈ�� �߰��Ѵ�
		HighScoringArray.AddUnique(PlayerHighScore);


	}
	else if (PlayerHighScore->GetScore() > HighScore)	// �ٸ� ���� ������ �ֱ� ���� �۾�
	{
		HighScoringArray.Empty();
		HighScoringArray.Emplace(PlayerHighScore);
		HighScore = PlayerHighScore->GetScore();

	}

}
