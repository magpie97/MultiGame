// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_UnEquipped UMETA(DisplayName = "UnEquipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),

	EFT_MAX UMETA(DisplayName = "DefaultMAX")

};

UCLASS()
class SHOOT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeapon();


public:

	// 무기 별 따로의 crosshair를 구현하기 위한 변수     xxx 캠버스로 그릴지 이미지로 처리할지 고민중 
	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	class UTexture2D* CrosshairCanter;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairLeft;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairRight;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairUp;

	UPROPERTY(EditAnywhere, Category = "Crosshairs")
	UTexture2D* CrosshairDown;

	// 총기 자동 발사

	// 다시 발사할 딜레이 변수
	UPROPERTY(EditAnywhere, Category = Combat)
	float FireDelay = 0.2f;			// default  0.2

	UPROPERTY(EditAnywhere, Category = Combat)
	bool bAutomatic = true;

	// latency 에 대응할 enum
	UPROPERTY(EditAnywhere)
	EFireType FireType;




	// 변수 섹션
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Propertie")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Propertie")
	class USphereComponent* AreaSphere;

	// ReplicatedUsing 은  네트워크를 통해 프로퍼티를 업데이트할 때 실행되는 콜백 함수를 지정한다			따라서 업데이트할 함수를 같이 할 set함수와 onRep_ 함수를 같이 만들어준다 
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	class UAnimationAsset* FireAnimations;


protected:
	/*
		조준하는 동안의 카메라의 fov 값을 위한 변수
	*/
	// 30.f
	UPROPERTY(EditAnywhere)
	float ZoomedFOV = 30.f;

	// 20.f
	UPROPERTY(EditAnywhere)
	float ZoomInterpSpeed = 20.f;

	// 카메라 줌 할때 위치 변경  test
	float InterpCameraChangeSpeed = 10.f;

	UPROPERTY(EditAnywhere)
	EWeaponTypes WeaponTypes;

	UPROPERTY(EditAnywhere, Category = Damage)
	float Damage = 0.f;





	/*
		캐릭터가 죽고 default weapon을 destory 하는 함수 및 변수 섹션
	*/
	
public:
	bool bDestroyDefaultWeapon = false;
	

	UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;



public:

	/*
	총알 관련 섹션
	*/
	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 Ammo;

	// 탄창 용량
	UPROPERTY(EditAnywhere, Category = "Ammo")
	int32 MagCapacity;

	// 총을 쏠때마다 총알 개수를 업데이트 하기 위한 함수
	UFUNCTION()
	void SpendRound();

	UFUNCTION()
	void AddAmmo(int32 AmmoToAdd);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AddToAmmo);


	// ammo에 대한 처리되지 않는 서버 요청 수
	int32 Sequence = 0;


	/*
		총 관련 재장전 및 픽업 사운드 섹션
	*/

	// 픽업 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundCue* EquipWeaponSound;


	bool IsFullAmmo();



private:

	UPROPERTY()
	class ABaseCharacter* OwnerBaseCharacter;

	UPROPERTY()
	class AShooterPlayerController* OwnerShooterPlayerController;






public:
	virtual void Fire(const FVector& HitTarget);

	UFUNCTION()
	void OnRep_WeaponState();


	// 함수 섹션
public:
	void ShowPickupWidget(bool bShowWidget);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 
	virtual void OnRep_Owner() override;

	void SetHUDAmmo();


	// 함수 섹션
protected:

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:
	// 캐릭터가 죽으면 무기를 떨어뜨린다
	void Dropped();




	// FORCEINLINE 섹션
public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	// 무기의 fov 값
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }

	// 총알이 0개
	bool IsEmpty();
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE EWeaponTypes GetWeaponTypes() const { return WeaponTypes; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
