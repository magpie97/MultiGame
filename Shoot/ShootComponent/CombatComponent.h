// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Shoot/Weapon/WeaponTypes.h"
#include "Shoot/ShootType/CombatState.h"
#include "CombatComponent.generated.h"



UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Initial UMETA(DisplayName = "Initial State"),
	ECS_Walk UMETA(DisplayName = "Walk"),
	ECS_Equipped UMETA(DisplayName = "Sprint"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX"),
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();

	friend class ABaseCharacter;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;




protected:
	virtual void BeginPlay() override;

	// 에임했을때 캐릭터의 속도를 줄이기 위한 함수
	UFUNCTION()
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	//============ sprint testing ===================== 
	UFUNCTION()
	void SetSprint(bool bIsSprint);

	UFUNCTION(Server, Reliable)
	void ServerSetSprint(bool bIsSprint);

	//=================================================

	


	//=========해당 하는 총기의 애니메이션 서버와 클라이언트 실행============

	//firing camera shake
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> CameraShake;

	// 클라이언트에서만 작동하는 함수 인데 그 아래 함수는 클라이언트가 서버에 보내는 RPC
	UFUNCTION()
	void FireButtonPressed(bool bPressed);

	UFUNCTION()
	void Fire();

	UFUNCTION()
	void FireProjectileWeapon();

	UFUNCTION()
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

	// 클라이언트에서 서버로 총을 발사하는 애니메이션은 작동하지만 클라이언트에서 보여지는건 없다 그리하여
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);

	// netmulticast 를 사용하여 모든 클라이언트에게 보여지게 한다
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	//==================================================================

	UFUNCTION()
	void EquipWeapon(class AWeapon* WeaponToEquip);

	UFUNCTION()
	void WeaponCarriedAmmo();

	UFUNCTION()
	void WeaponPickupSound();

	UFUNCTION()
	void ReloadWeapon();

	UFUNCTION() // 무기 오른손 소켓
	void AttachActorToRightHandSocket(AActor* ActortoAttach);

	UFUNCTION() // 수류탄 전용 소켓
	void AttachActorToLeftHandSocket(AActor* ActorToAttach);

	UFUNCTION()
	void ShowGrenadeMesh(bool visible);

	UFUNCTION()
	void WeaponDropped();

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	UFUNCTION()
	void SetHUDCrosshairs(float DeltaTime);




	


public: // defualt private
	UPROPERTY(meta = (AllowPrivateAccess = "true"))
	class ABaseCharacter* Character;

	UPROPERTY()
	class AShooterPlayerController* Controller;

	UPROPERTY()
	class AShooterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon, meta = (AllowPrivateAccess = "true"))		// replicatedusing 은 서버와 클라이언트의 동기화 하기 위한 요소임
	class AWeapon* EquippedWeapon;

	UPROPERTY()
	ECharacterState CharacterState;







private:
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming;

	// 조준 버튼을 눌렀는지 확인하려는 변수   클라이언트에서 확인할려는 변수이다
	UPROPERTY()
	bool bAimingButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();



	UPROPERTY(Replicated, meta = (AllowPrivateAccess = "true"))
	bool bSprint;

	UPROPERTY(Replicated)
	bool bJump;

	UPROPERTY(EditAnywhere, Category = "Sprint")
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, Category = "Sprint")
	float AimWalkSpeed;

	// test variable
	UPROPERTY(EditAnywhere, Category = "Sprint")
	float SprintSpeed;

	// 무기 발사 판단 변수
	UPROPERTY()
	bool bFireButtonPressed;



	// 임펙포인트를 위한 변수
	UPROPERTY()
	FVector HitTarget;

public:
	




public:
	// 타이머에 사용할 handle
	UPROPERTY()
	FTimerHandle FireTimeHandle;

	UPROPERTY(Replicated)
	bool bCanFire = true;

	UFUNCTION()
	void StartFireTimer();

	UFUNCTION()
	void EndFireTimer();

	bool CanFire();




	/*
		총알 관력 섹션
	*/


	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	// map은 복사가 안된다   복사할 이유가 없는 carriedammo 때문에 map을 사용
	UPROPERTY(EditAnywhere)
	TMap<EWeaponTypes, int> CarriedAmmoMap;

	// test
	UPROPERTY(EditAnywhere)
	int32 StartPistolAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartRifleAmmo = 0;

	UPROPERTY(EditAnywhere)
	int32 StartGrenade = 0;

	UFUNCTION()
	void InitCarriedAmmo();

	UFUNCTION()
	void Reload();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
	void OnRep_CombatState();

	UFUNCTION()
	void HandleReload();

	UFUNCTION(BlueprintCallable)
	void FinishReload();

	// 무기의 총알
	UFUNCTION()
	int32 AmountToReload();

	UFUNCTION()
	void UpdateAmmoValue();



	// 수류탄 섹션
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;

	// 수류탄 개수
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CarriedGrenade)
	int32 CarriedGrenade = 3;

	UFUNCTION()
	void OnRep_CarriedGrenade();

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedGrenade = 3;

	UFUNCTION()
	bool IsGrenadeEmpty();

	UFUNCTION()
	void UpdateCarriedGrenade();


public:

	//UPROPERTY()
	//float tm = 0.f;
	// 수류탄 handle
	UPROPERTY()
	FTimerHandle CookingTime;

	// 블프에서 세팅
	UPROPERTY(EditAnywhere)
	float CookingDelay;

	UPROPERTY()
	bool bCanThrowGrenade = false;

	//default 0.f
	UPROPERTY()
	float GrenadeHoldTime = 0.f;

	UFUNCTION()
	bool CanThrowGrenade();

	// 홀드
	UFUNCTION()
	void ThrowGrenadeHold(bool bGrenade);

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenadeHold();

	UFUNCTION(NetMulticast, Reliable)
	void MultiThrowGrenadeHold();

	// 블루프린트에서 사용
	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeHoldFinished();



	// 수류탄 던지기 
	UFUNCTION()
	void ThrowGrenade(bool bGrenade);

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UFUNCTION(BlueprintCallable)
	void GrenadeStart();

	UFUNCTION(Server, Reliable)
	void ServerGrenadeStart(const FVector_NetQuantize& Target);

	/*UFUNCTION(NetMulticast, Reliable)
	void MultiGrenadeStart(const FVector_NetQuantize& Target);*/

	UFUNCTION(BlueprintCallable)
	void GrenadeFinished();




	/*
		조준 및 카메라 FOV 변수
	*/
	UPROPERTY()
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomFOV = 80.f;			// default value 30.f

	UPROPERTY(EditAnywhere, Category = Combat)
	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;	// 20.f

	// 카메라 fov 보간 처리
	UFUNCTION()
	void InterpFOV(float DeltaTime);


	UPROPERTY()
	FVector DefaultCameraLocation;
	UPROPERTY()
	FVector CurrentCameraLocation;

	UPROPERTY()
	FVector FirstCamera;
	UPROPERTY()
	FVector SecondCamera;

	// 카메라 보간 처리 
	UFUNCTION()
	void InterpCamera(float Delta);


public:
	FORCEINLINE int32 GetCarriedGrenade() const { return CarriedGrenade; }



public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
