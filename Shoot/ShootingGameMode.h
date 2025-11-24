// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Shoot/Character/BaseCharacter.h"

#include "ShootingGameMode.generated.h"


// 커스텀 게임 모드 
namespace MatchState
{
	extern SHOOT_API const FName WaitNextGame;
}


UCLASS()
class SHOOT_API AShootingGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AShootingGameMode();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;





public:
	// GameState 및 Blueprint가 SetMatchState 호출을 받기 전에 적절한 전환 함수를 전달하는 재정의 가능한 가상 함수
	UFUNCTION()
	virtual void OnMatchStateSet() override;

	UPROPERTY()
	bool PressedTabInfo = false;










	/*
	* chat system
	*/
	UPROPERTY()
	TSubclassOf<class ABaseCharacter> BaseCharacterClass;
	
	UPROPERTY()
	TSubclassOf<class AShooterPlayerController> ShooterPlayerControllerClass;
	
	UPROPERTY()
	TSubclassOf<class AShooterHUD> ShooterHUDClass;

	UPROPERTY()
	class AShooterPlayerController* ShooterPlayerController;
	



public:

	/*
		player가 제거 될때 호출할 함수
		제거 되는 캐릭터를 알아야 하기에 입력 파라미터를 추가한다
		그리고 제거되는 캐릭터의 컨트롤러와 제거되게하는 피의자 캐릭터 컨트롤러가도 필요하다
	*/
	virtual void PlayerDead(class ABaseCharacter* DeadCharacter, class AShooterPlayerController* VictimPlayerController, AShooterPlayerController* AttackerPlayerController);

	// 유저가 죽었다면 다시 생성하기위해 해당 캐릭터, 컨트롤러를 준다
	virtual void PlayerRespawn(ACharacter* DeadCharacter, AController* DeadController);



	virtual void PlayerKillStreak(AShooterPlayerController* AttackerPlayerController);

	int32 KillStreakCount(AShooterPlayerController* AttackerPlayerController);



	// 게임 시작 전 10초
	UPROPERTY(EditDefaultsOnly)
	float WaitTime = 5.f;	

	// 게임진행 120초
	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 10.f;

	// 다음 게임 10초 대기
	UPROPERTY(EditDefaultsOnly)
	float WaitNextGameTime = 5.f;

	// 게임 상태를 변경할 변수인 변수 0.f 초
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CountdownTime = 0.f;

	// 0.f 초  begine 함수에서 0의 시간으로 초기화
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float LevelStartingTime = 0.f;



	// kill streak
	FTimerHandle KillStreakTimerHandle;

	UPROPERTY(EditAnywhere, Category = "KillStreakResetTime")
	float ResetTime = 5.f;

	UFUNCTION()
	void ResetKillStreakScore();

	float KillScore = 0;


	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }


private:


	
};
