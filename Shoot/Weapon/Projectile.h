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

	// projectile destroy�� ���� �Լ�     �������Ʈ���� OnComponentHit �̶�� �̸����� ���ȴ� 
	UFUNCTION()
	virtual void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);







public:

	UPROPERTY(EditAnywhere)
	class ABaseCharacter* Character;

	UPROPERTY(EditAnywhere)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
	class USceneComponent* ScenRoot;

	// �Ѿ��� �浹�� ó���� ������Ʈ  test
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* CollisionCapsule;

	// �Ѿ��� �浹�� ó���� ������Ʈ  test
	UPROPERTY(EditAnywhere)
	class USphereComponent* CollisionSphere;

	// �Ѿ��� �浹�� ó���� ������Ʈ 
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




	//====== Server side rewind ���� ========

	// �Ѿ� ������
	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage = 0.f;

	//UPROPERTY(Replicated, EditAnywhere)
	bool bUseServerSideRewind = false;

	// ���� ���� ��ġ
	UPROPERTY()
	FVector_NetQuantize TraceStart;		// �Ҽ����� �������� �ʴ� ����

	// �ӷ�
	UPROPERTY()
	FVector_NetQuantize100 InitVelocity;	// FVector_NetQuantize100 �� �Ҽ��� ���� ����ϱ� ���� ���� �̴�

	// ����ü �ӵ�  (������ 10000 ���� �ߴµ� ��ƼŬ�� ������ ���ϴ� ����� �𿩼� )
	UPROPERTY(EditAnywhere)
	float InitSpeed = 15000.f;

	//====== Server side rewind ���� ========







	//=====����ź ����
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ProjectileGrenadeMesh;

	// ���󰡴� ���� ����
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem;

	UPROPERTY(EditAnywhere)
	class UNiagaraComponent* TrailSystemComponent;
	
	UFUNCTION()
	void SpawnTrailSystem();

	FTimerHandle DestroyTimeHandle;

	// ����ź ���ð�
	// ����ź ���ð� 3.f
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
