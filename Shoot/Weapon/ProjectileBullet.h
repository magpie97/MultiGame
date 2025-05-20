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


	// �����Ϳ����� ����ϱ⵵ �ϸ� ��Ÿ�Ӷ� ���ʿ��� ������ �ڵ带 ���� ���� ������ �ð��� �ٿ� ����ȭ ��Ų�� 
#if WITH_EDITOR
	// ������ ������ ���� �Ӽ����� ���� �ɶ�  ����� �Ӽ� ���� ���� �Ӽ� ���� ���� �ϴ� �Լ��̴� 
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
