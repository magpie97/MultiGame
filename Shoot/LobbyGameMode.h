// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShootGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API ALobbyGameMode : public AShootGameMode
{
	GENERATED_BODY()
public:

	// 현재 맵의 플레이어가 몇명 있는지 카운트
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// PlayerState가 있는 Controller가 게임을 떠나거나 파괴될 때 호출
	virtual void Logout(AController* Exiting) override;
	
private:



public:

	UPROPERTY()
	class UShootGameInstance* ShootGameInstance;

	// 플레이어 수 계산
	uint16 NumberOfPlayers = 0;

	//=======================================
	// 로비에서 사용할 변수		// test
	uint16 CanJoinMaxNumber = 2;

	// testing
	FORCEINLINE uint16 GetNumberOfPlayers() { return NumberOfPlayers; }
	//=======================================

};
