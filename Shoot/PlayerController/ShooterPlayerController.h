// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Shoot/ChatSystem/ChatPlayerController.h"
#include "ShooterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AShooterPlayerController();

	UFUNCTION()
	void SetHUDHealth(float Health, float MaxHealth);

	UFUNCTION()
	void SetHUDKillScore(float KillScore);
	
	UFUNCTION()
	void SetHUDDeathScore(int32 DeathScore);


	// chat system
	UPROPERTY()
	class UChatPlayerController* ChatPlayerController;



	// ==== test code   작동하는듯

	// killcount
	//int32 kc = 0;


	FTimerHandle KillStreakTimerHandle;
	FTimerHandle DelayKillStreakTimerHandle;

	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* FirstKill;
	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* FirstKill_Voice;
	
		
	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* DoubleKill;
	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* DoubleKill_Voice;


	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* TripleKill;
	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* TripleKill_Voice;


	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* QuadraKill;
	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* QuadraKill_Voice;


	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* Pentakill;
	UPROPERTY(EditAnywhere, Category = "KillStreakSound")
	class USoundCue* Pentakill_Voice;

	// 클라 끼리만 들리게 하기
	UFUNCTION(Client, Reliable)
	void ClientNotifyKillStreak(int32 KillCount);

	// ===================

	
	UFUNCTION()
	void SetHUDWeaponAmmo(int32 Ammo);
	
	UFUNCTION()
	void SetHUDCarriedAmmo(int32 Ammo);

	// 수류탄 개수 hud
	UFUNCTION()
	void SetHUDCarriedGrenade(int32 Grenade);

	// 매치 타이머
	UFUNCTION()
	void SetHUDMatchCountdown(float Countdowntime);

	// 게임 시작 전 카운트 다운 기능
	UFUNCTION()
	void SetHUDPrepareToStartCountDownTime(float PrepareToStartCountDownTime);

	UFUNCTION()
	virtual float GetServerTime();

	/** 이 플레이어 컨트롤러의 뷰포트/넷 연결이 이 플레이어 컨트롤러와 연결된 후 호출됩니다. */
	// 가능한 빨리 서버와 시간 동기화 하기 위한 함수
	UFUNCTION()
	virtual void ReceivedPlayer() override;

	UFUNCTION()
	void OnMatchStateSet(FName State);


	// hide widget
	UFUNCTION()
	void HandlePrepareToAttackWidget();

	UFUNCTION()
	void HandleWaitNextGameStateWidget();

	// 게임모드클래스에서 시간 동기화가 이루어져야 할것이 플레이어 컨트롤러에 시간 동기화가 이루어지고 있다 따라서 게임모드에서 시간을 받아올 작업을 한다 
	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidGame(FName MS, float WT, float MT, float LST, float WNGT);






	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void Tick(float DeltaTime) override;



protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

public:
	UFUNCTION()
	void SetHUDTime();



	/*
		서버, 클라에 동일 한 시간을 동기화 하기 위한 작업
	*/
	// 현재 서버 시간을 요청한다 요청이 전송된 클라이언트 시간을 전달
	UFUNCTION(Server, Reliable)
	void SRequestServerTime(float TimeOfClientRequest);	//앞에 Server는 접두어처럼 썼음

	// ServerRequestServerTime에 대한 응답으로 현재 서버 시간을 클라이언트에 보고
	UFUNCTION(Client, Reliable)
	void CReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	UPROPERTY()
	float ClientServerDelta = 0.f;	// 클라이언트와 서버 시간 간의 차이

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency = 5.0f;
	
	UPROPERTY()
	float TimeSyncRunningTime = 0.f;

	UFUNCTION()
	void CheckTimeSync(float DeltaTime);

	// Character Overlay init func
	UFUNCTION()
	void PollInit();

	// 게임시작전, 게임시작후, 게임시끝 게임플레이중에 추가로 서버에 유저가 들어온다면 새로운 유저는 게임 상태를 알 수없다
	// 따라서 RPC를 이용하여 playercontroller가 게임상태를 알수있게 할 기능이 필요하다
	/*UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();*/




	//서버,클라 시간 동기화 섹션
private:

	UPROPERTY()
	class AShooterHUD* ShooterHUD;

	UPROPERTY()
	class UPlayerScoreWidget* PlayerScoreWidget;

	UPROPERTY()
	class AWeapon* Weapon;	// test

	// 매치 시간 180초
	UPROPERTY()
	float MatchTime = 0.f;

	// 0.f   게임시작 하기 전 카운트
	UPROPERTY()
	float WatingTime = 0.f;

	// 0.f
	UPROPERTY()
	float LevelStartingTime = 0.f;

	// 게임 끝난 이후의 시간
	UPROPERTY()
	float WaitNextGameTime = 0;

	// 0초
	UPROPERTY()
	uint32 CountdownInt = 0;


	// 게임 시작하기 카운트 애니메이션
	UPROPERTY(EditAnywhere, Category = "WidgetAnimation", Transient)
	class UWidgetAnimation* CountdownAnimation;

public:
	float SingleTripTime = 0;


public:
	UPROPERTY()
	class AShootingGameMode* ShootingGameMode;

	// 클라이언트에도 복사
	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();


public:




	/*
		네트워크  ping check 
	*/

	//UPROPERTY()
	//class AShooterPlayerState* ShooterPlayerState;


	/* 
		캐릭터의 채력이 (퍼센트 위젯이 작동이 게임시작시 100%로 보여지지 않는 문제가 보여 추가한 변수)  	
	*/
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY()
	bool bInitCharacterOverlay = false;

	UPROPERTY()
	float HUDCurrentHealth;
	UPROPERTY()
	bool bInitHUDCurrentHealth = false;

	UPROPERTY()
	float HUDMaxHealth;
	UPROPERTY()
	bool bInitHUDMaxHealth = false;

	UPROPERTY()
	float HUDKillScore;
	UPROPERTY()
	bool bInitHUDKillScore = false;

	UPROPERTY()
	int32 HUDDeathScore;
	UPROPERTY()
	bool bInitHUDDeadthScore = false;

	UPROPERTY()
	int32 HUDGrenade;
	UPROPERTY()
	bool bInitHUDGrenade = false;

	UPROPERTY()
	float HUDCarriedAmmo;
	UPROPERTY()
	bool bInitHUDCarriedAmmo = false;

	UPROPERTY()
	float HUDWeaponAmmo;
	UPROPERTY()
	bool bInitHUDWeaponAmmo = false;




	/* Player Kill Feed*/
public:

	void Announcement_KillFeed(APlayerState* SuspectUserState, APlayerState* VictimUserState);

protected:

	// 클라이언트 표시
	UFUNCTION(Client, Reliable)
	void ClientAnnouncement_KillFeed(APlayerState* SuspectUserState, APlayerState* VictimUserState);


};
