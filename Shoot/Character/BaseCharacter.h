// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Shoot/ShootType/ShootType.h"
#include "Shoot/ShootType/CombatState.h"
#include "BaseCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class UAnimMontage;

UCLASS()
class SHOOT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter(const FObjectInitializer& ObjInit);

	// 변수 섹션
private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* NextCamera;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class ABaseCharacter* BaseCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* WidgetComponentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UOverheadWidget* OverheadWidget;

	// 수류탄 매시
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* GrenadeMesh;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class AShooterPlayerController* ShooterPlayerController;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	class AShootingGameMode* GameMode;







private:
	/*
		머리
	*/
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxHead;

	/*
		몸통 
	*/
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxPelvis;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxSpine_1;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxSpine_2;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxSpine_3;

	/*
		팔
	*/
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxUpperArm_L;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxLowerArm_L;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxUpperArm_R;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxLowerArm_R;

	/*
		다리
	*/
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxUpperLag_L;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxLowerLag_L;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxUpperLag_R;
	UPROPERTY(EditAnywhere)
	class UBoxComponent* HitBoxLowerLag_R;

public:

	// 모든 박스 콜리전 요소를 저장할 컨테이너 (생성자에서 저장함)
	UPROPERTY()
	TMap<FName, UBoxComponent*> HitBoxesMap;

	// 캐릭터의 액터 컴포넌트를 접근 하기 위해 만듬
	UPROPERTY(VisibleAnywhere)
	class UServerSideRewindComponent* ServerSideRewindComponent;




public:   // default 원래 없었음

	// 레플리케이트
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	// 전투 관련 모든 관련된 기능을 처리할 변수 선언
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	// aimoffset 계산 전용 변수
	UPROPERTY()
	float AO_Yaw;

	UPROPERTY()
	float InterpAO_Yaw;

	UPROPERTY()
	float AO_Pitch;

	// 45에서 -45각도에서 turning 하기 위한 기능
	UPROPERTY()
	ETurningInPlace TurningInPlace;

	UFUNCTION()
	void TurnInPlace(float DeltaTime);



public:

	/*
		몽타주 섹션
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	// 캐릭터가 죽는 몽타주
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* DeadMontage;

	// Rifle 재장전 몽타주
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	// 수류탄 홀드 몽타주
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeHoldMontage;

	// 수류탄 던지기 몽타주
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGrenadeMontage;

	// 캐릭터 슬라이딩 몽타주
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SlidingMontage;  //test







	// 캐릭터의 뼈대를 돌릴지 안돌릴지를 판단할 변수
	UPROPERTY()
	bool bRotateRootBone;
	// 회전의 임계값
	UPROPERTY()
	float TurnThreshold = 0.f;
	// 마지막 회전값
	UPROPERTY()
	FRotator ProxyRotationLastFrame;
	// 현재 회전값
	UPROPERTY()
	FRotator ProxyRotationCurrentFrame;
	// Yaw 회전값
	UPROPERTY()
	float ProxyYaw;
	// 캐릭터가 멈추면  delegate를 호출을 하지 않을것이다 그래서 마지막 replicatedmovement 이후의 시간을 추적하고 그 특정시간에 도달하면 일부 프록시를 호출하고 
	// 반 정기적으로 호출하는지  확인하기 위한 변수이다
	UPROPERTY()
	float TimeSinceLastMovementReplication;


public:




	/*
		플레이어의 채력 관리
	*/
	// 채력이 줄어든 시점부터 딜레이 시간
	FTimerHandle AutoHealDelayHandle;

	// 체력이 줄어든 시점에 힐링 시간
	FTimerHandle AutoHealTimeHandle;

	UPROPERTY(EditAnywhere, Category = "Player State")
	float MaxHealth = 100.f;
	
	// 현재 체력을 복사할것
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Health, Category = "Player State")
	float CurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Player State")
	bool bIsAutoHeal = false;

	// hp 회복
	UPROPERTY(EditAnywhere, Category = "Player State")
	float AutoHealValue = 0.f;

	// 맞은 직후 실행 시간
	UPROPERTY(EditAnywhere, Category = "Player State")
	float AfterHit = 0.f;

	// 5초 뒤 회복
	UPROPERTY(EditAnywhere, Category = "Player State")
	float AutoHealDelayTime = 0.f;

	// 5초 반복 회복 딜레이
	UPROPERTY(EditAnywhere, Category = "Player State")
	float AutoHealTime = 0.f;

	UFUNCTION()
	void AutoHealingTime();

	UFUNCTION()
	bool IsFullHealth();

	UFUNCTION()
	void AutoHealingDelay();



	// 체력 부족 및 제압 화면
	UPROPERTY(EditAnywhere, Category = "DOF&Chromatic")
	float DOFInterpSpeed = 0.f;

	UPROPERTY(EditAnywhere, Category = "DOF&Chromatic")
	float UpdateDOFValue = 10000.f;

	UPROPERTY(EditAnywhere, Category = "DOF&Chromatic")
	float UpdateChromaticValue = 0.f;

	UFUNCTION()
	void LowHealthEffect(float DeltaTime);





	//UPROPERTY(Replicated)
	bool bDead = false;

	FTimerHandle DeadTimer;

	UPROPERTY(EditDefaultsOnly, Category = "DeadTimeDelay")
	float DeadDelay = 4.3f;

	UFUNCTION()
	void DeadTimerFinished();

	// press TAB key on showing players info
	bool bShowInfo = false;

	UPROPERTY()
	class AShooterPlayerState* ShooterPlayerState;

	// 게임이 끝나고 WaitNextGame 상태가 되면 캐릭터의 입력을 중지 시킬때 사용
	UPROPERTY(Replicated)
	bool bDisableGamePlay = false;




private:

	// physical animation 섹션
	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UPhysicalAnimationComponent* PhysicalAnimationComponent;







public:

	UFUNCTION()
	void PlayFireMontage(bool bAiming);

	UFUNCTION()
	void PlayDeadMontage();

	UFUNCTION()
	void PlayHitReactMontage();

	UFUNCTION()
	void PlayReloadMontage();


	UFUNCTION()
	void PlayThrowGrenadeHoldMontage();

	UFUNCTION()
	void PlayThrowGrenadeMontage();





	// 구조체 복제 이벤트 라고 한다
	virtual void OnRep_ReplicatedMovement() override;


public:
	// aim offset를 animinstance에 써야하기때문에 inline 함수를 사용
	FORCEINLINE ABaseCharacter* GetCharacter() const { return BaseCharacter; }
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE UCameraComponent* GetSecondCamera() const { return NextCamera; }
	FORCEINLINE bool GetRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool GetDead() const { return bDead; }
	FORCEINLINE float GetHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE UOverheadWidget* GetOverheadWidget() const { return OverheadWidget; }
	FORCEINLINE UStaticMeshComponent* GetGrenadeMesh() const { return GrenadeMesh; }
	FORCEINLINE bool GetGrenadeHold() const { return bGrenadeHold; }
	FORCEINLINE AShooterPlayerController* GetShooterPlayerController() const { return ShooterPlayerController; }
	FORCEINLINE UServerSideRewindComponent* GetServerSideRewindComponent() const { return ServerSideRewindComponent; }
	
	



	ECombatState GetCombatState() const;

	// 디버깅하기 위한 함수
	FVector GetHitTarget() const;

	AWeapon* GetEquippedWeapon();

	FRotator StartingAimRotation;

	// 클라이언트를 호출해 서버에서 실행할 함수
	UFUNCTION(Server, Reliable)	// RBC를 만들때 이 RBC가 믿을 만한지 아닌지 명시해 줘야 한다
	void ServerEquipButtonPressed();

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);



	 
public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();

protected:
	virtual void BeginPlay() override;

	// Character Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	virtual void Jump() override;

	// 게임 시작시 기본 장착된 무기 설정
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

	UFUNCTION()
	void AttachDefaultWeapon();


	// camera change location
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	bool bSecondCamera;

	//void CameraSwitch();
	UFUNCTION()
	void FireButtonPressed();

	UFUNCTION()
	void FireButtonReleased();



	// 수류탄 던지기 애니메이션 홀드
	UFUNCTION()
	void GrenadeButtonPressed();

	UFUNCTION()
	void GrenadeButtonReleased();



	UFUNCTION()
	void OnRep_Health();


	/*
		Damage 처리 할 함수
	*/
	UPROPERTY()
	bool bHit = false;

	UFUNCTION()
	void ApplyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	UFUNCTION()
	void UpdateHUDHealth();

	UFUNCTION()
	void UpdateHUDWeaponAmmo();

	// 플레이어의 killscore를 게임시작시 0으로 초기화
	UFUNCTION()
	void PollInit();


	


public:
	// aimming
	UFUNCTION()
	void CrouchButtonPressed();

	UFUNCTION()
	void AimButtonPressed();

	UFUNCTION()
	void AimButtonReleased();

	UFUNCTION()
	bool IsAiming();

	UFUNCTION()
	void AimOffset(float DeltaTime);

	UFUNCTION()
	void CalculateAO_Pitch();

	// 클라이언트에서 서버로 서버에서 클라이언트로 캐릭터의 부드러운 움직임을 처리할 함수
	UFUNCTION()
	void SimProxiesTurn();

	UFUNCTION()
	float CalculateSpeed();




	// sprint
	UFUNCTION()
	void SprintButtonPressed();

	UFUNCTION()
	void SprintButtonReleased();

	UPROPERTY(Replicated)
	bool bSprint = false;

	UPROPERTY(Replicated)
	bool bJump = false;

	UFUNCTION()
	bool IsSprint();

	UPROPERTY()
	bool bGrenadeHold = false;

	UPROPERTY()
	bool bMoveForward = false;


	// sliding

	/*bool bSliding = false;

	bool bIsSliding(bool Sliding);

	void SlidingButtonPressed(bool Sliding);

	void SlidingButtonReleased(bool Sliding);*/



	// Dead
	//UFUNCTION(Client, Reliable)
	void Dead();

	// test server
	UFUNCTION(Server, Reliable)
	void ServerDead();


	// 캐릭터가 죽는 애니메이션을 서버와 클라이언트를 동기화   (클라이언트를 위한 작업)
	UFUNCTION(NetMulticast, Reliable)
	void MulticastDead();

	UPROPERTY(EditAnywhere)
	class USoundCue* DeadSound;

	
	// 재장전
	UFUNCTION()
	void ReloadButtonPressed();


	/*
		정보창 Tab key 유저 이름, 점수,,, 등등   %%% 아직 구현하지 않음  위젯은 만들어 둠
	*/
	UFUNCTION()
	void InfoButtonPressed();

	UFUNCTION()
	void Info();




public:

	// hitreaction 섹션
	UFUNCTION()
	void PlayPhysicalAnimation();








public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// PostInitializeComponents 컴포넌트 초기화 이후 - 액터의 컴포넌트 초기화가 완료된 이후 호출됩니다.
	virtual void PostInitializeComponents() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
