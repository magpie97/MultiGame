// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()


public:
	virtual void Fire(const FVector& HitTarget) override;

	
private:
	// 해당하는 무기의 총알을 손 쉽게 변경 가능하게 만들 컨테이너 중 하나
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ServerSideRewindProjectileClass;

	//firing camera shake
	/*UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> CameraShake;*/

	
};
