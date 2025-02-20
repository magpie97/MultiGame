// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

/**
 * 
 */

class USoundCue;

UCLASS()
class SHOOT_API AProjectileGrenade : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileGrenade();

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;



	UFUNCTION()
	void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;



public:



private:
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* GrenadeMeshComponent;

	//class USkeletalMeshComponent* GrenadeMeshComponent;

	UPROPERTY()
	class AProjectileGrenade* Grenade;





public:

	// 수류탄이 벽에 팅기거나 구를때 필요한 사운드
	UPROPERTY(EditAnywhere)
	USoundCue* BounceSound;

	UPROPERTY(EditAnywhere)
	float Speed = 0.f;







	
};
