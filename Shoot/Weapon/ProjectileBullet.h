// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API AProjectileBullet : public AProjectile
{
	GENERATED_BODY()
	

public:

	AProjectileBullet();



private:

	UPROPERTY(EditAnywhere, Category = "Weapon")
	class AWeapon* Weapon;




protected:
	virtual void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual void BeginPlay() override;


	// 에디터에서만 사용하기도 하며 런타임때 불필요한 에디터 코드를 제외 시켜 컴파일 시간을 줄여 최적화 시킨다 
#if WITH_EDITOR
	// 에디터 디테일 단일 속성값이 변경 될때  변경된 속성 값을 단일 속성 값을 변경 하는 함수이다 
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
