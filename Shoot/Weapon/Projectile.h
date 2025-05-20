// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"






UCLASS()
class SHOOT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	// projectile destroy를 위한 함수     블루프린트에선 OnComponentHit 이라는 이름으로 사용된다 
	UFUNCTION()
	virtual void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);







public:

	UPROPERTY(EditAnywhere)
	class ABaseCharacter* Character;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class USceneComponent* ScenRoot;

	// 총알의 충돌을 처리할 컴포넌트  test
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* CollisionCapsule;

	// 총알의 충돌을 처리할 컴포넌트  test
	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionSphere;

	// 총알의 충돌을 처리할 컴포넌트 
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ParticleSystem;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere)
	UParticleSystem* ImpactParticle;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	float SoundVolum;




	//====== Server side rewind 섹션 ========

	// 총알 데미지
	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 0.f;

	//UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;

	// 시작 추적 위치
	UPROPERTY()
	FVector_NetQuantize TraceStart;		// 소수점을 포함하지 않는 벡터

	// 속력
	UPROPERTY()
	FVector_NetQuantize100 InitVelocity;	// FVector_NetQuantize100 은 소수점 까지 사용하기 위한 벡터 이다

	// 투사체 속도  (원래는 10000 정도 했는데 파티클이 따라가지 못하는 모습이 모여서 )
	UPROPERTY(EditAnywhere)
	float InitSpeed = 15000.f;

	//====== Server side rewind 섹션 ========







	//=====수류탄 섹션
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ProjectileGrenadeMesh;

	// 날라가는 궤적 꼬리
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* TrailSystemComponent;
	
	UFUNCTION()
	void SpawnTrailSystem();

	FTimerHandle DestroyTimeHandle;

	// 수류탄 대기시간
	// 수류탄 대기시간 3.f
	UPROPERTY(EditAnywhere)
	float BlowupTimeToGrenade = 3.f;

	UPROPERTY(EditAnywhere)
	float DestoryProjectileTime = 3.f;

	UPROPERTY(EditAnywhere)
	float GrenadeDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float MinDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float MaxDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float DamageInnerRadius = 0.f;

	UPROPERTY(EditAnywhere)
	float DamageOuterRadius = 0.f;




public:

	UFUNCTION()
	void DestoryProjectileStart();

	UFUNCTION()
	void DestoryProjectileFinished();

	UFUNCTION()
	void ExplodeDamage();
	// ==========






public:	
	virtual void Tick(float DeltaTime) override;




};
