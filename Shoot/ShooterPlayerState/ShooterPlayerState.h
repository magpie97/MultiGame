
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

	
	


	
};
