// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/Weapon/Weapon.h"


AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitSpeed;
	ProjectileMovementComponent->MaxSpeed = InitSpeed;
	//ProjectileMovementComponent->ProjectileGravityScale = 0.1;
	
}


void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	FPredictProjectilePathParams ProjectilePath;
	FPredictProjectilePathResult ProjectileResult;
	ProjectilePath.bTraceWithChannel = true;
	ProjectilePath.bTraceWithCollision = true;
	ProjectilePath.DrawDebugTime = 3.f;
	ProjectilePath.DrawDebugType = EDrawDebugTrace::ForDuration;
	ProjectilePath.LaunchVelocity = GetActorForwardVector() * InitSpeed;
	ProjectilePath.MaxSimTime = 4.f;
	ProjectilePath.ProjectileRadius = 2.f;
	ProjectilePath.SimFrequency = 10.f;
	ProjectilePath.StartLocation = GetActorLocation();
	ProjectilePath.TraceChannel = ECollisionChannel::ECC_Visibility;
	ProjectilePath.ActorsToIgnore.Add(this);

	UGameplayStatics::PredictProjectilePath(this, ProjectilePath, ProjectileResult);

}

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 캐릭터가 총을 가지고 총은 투사체를 가지고 투사체의 데미지를 처리할 것인데
	// 액터를 소유한 액터를 가져오는 것이 GetOwner 함수이다 따라서 최종적으로 가져와야할 포인터는 character의 포인터인 것이다
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	class AWeapon* weapon = Cast<AWeapon>(GetOwner());

	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			//UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			

		}
	}

	Super::OnHit(HitComp, OtherActor,  OtherComp, NormalImpulse, Hit);

}

#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		FName Name = PropertyChangedEvent.GetPropertyName();

		if (Name == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitSpeed))
		{
			if (ProjectileMovementComponent)
			{
				ProjectileMovementComponent->InitialSpeed = InitSpeed;
				ProjectileMovementComponent->MaxSpeed = InitSpeed;

			}
		}
	}
}
#endif