// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerState.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/HUD/CharacterOverlay.h"
#include "Net/UnrealNetwork.h"


void AShooterPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// kill 스코어는 state 클래스 내부 변수는 이미 replicated 하기에 kill score는 복사할 필요 없다 
	DOREPLIFETIME(AShooterPlayerState, death);


}

// 클라이언트에 표시
void AShooterPlayerState::AddToScore(float ScoreAmount)
{

	// Score는 state class 내부에 선언된 변수이다
	//Score += ScoreAmount;
	
	// 이 인스턴스에 다른 부작용을 일으키지 않고 Score 값을 설정하는 함수
	// 위 하드코딩된건 문제없이 작동하지만 playerstate 클래스에서 사용할 수 있는 set 함수를 사용
	SetScore(GetScore() + ScoreAmount);

	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDKillScore(GetScore());


		}
	}

	
}

// 서버에 복사
void AShooterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;

	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDKillScore(GetScore());




		}
	}
}

// 클라이언트
void AShooterPlayerState::AddToDeathScore(uint32 DeathScoreAmount)
{
	death += DeathScoreAmount;

	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDDeathScore(death);

		}
	}
}

// 서버에 복사
void AShooterPlayerState::OnRep_DeathScore()
{
	Character = Character == nullptr ? Cast<ABaseCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;

		if (Controller)
		{
			Controller->SetHUDDeathScore(death);

		}
	}

}
