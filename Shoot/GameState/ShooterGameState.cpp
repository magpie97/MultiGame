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
		// 유저 점수중 제일 높은 점수를 add 함수로 저장 (Emplace 함수를 리턴함)
		HighScoringArray.Add(PlayerHighScore);
		HighScore = PlayerHighScore->GetScore();

	}
	else if (PlayerHighScore->GetScore() == HighScore)
	{
		// 기존 컨테이너에 동일한 엘리먼트가 이미 존재하지 않는 경우 새 엘리먼트만 추가한다
		HighScoringArray.AddUnique(PlayerHighScore);


	}
	else if (PlayerHighScore->GetScore() > HighScore)	// 다른 높은 점수를 넣기 위한 작업
	{
		HighScoringArray.Empty();
		HighScoringArray.Emplace(PlayerHighScore);
		HighScore = PlayerHighScore->GetScore();

	}

}
