
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ShooterPlayerState.generated.h"


UCLASS()
class SHOOT_API AShooterPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void OnRep_Score() override;

	UFUNCTION()
	void AddToScore(float ScoreAmount);

	UFUNCTION()
	virtual void OnRep_DeathScore();

	UFUNCTION()
	void AddToDeathScore(uint32 DeathScoreAmount);

	// state 클래스에 이미 정의됨 따라서 이건 안씀
	UFUNCTION()
	void ShowPing();

private:
	UPROPERTY()
	class ABaseCharacter* Character;
	
	UPROPERTY()
	class AShooterPlayerController* Controller;

	// getlifetimereplicatedprops에 사용할것이기에 매크로에 ReplicatedUsing을 사용
	UPROPERTY(ReplicatedUsing = OnRep_DeathScore)
	uint32 death;



public:

	// 현재 킬스트릭 카운트
	UPROPERTY(Replicated)
	int32 CurrentKillStreakCount = 0;

	// 킬스트릭 카운트 증가 및 타이머 시작/초기화 함수 (서버에서 호출됨)
	void IncrementKillStreak(float ResetTime);

private:
	FTimerHandle KillStreakResetTimerHandle;
	void ResetKillStreak();

	

	


	
};
