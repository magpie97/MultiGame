// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/Weapon/Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"


AProjectileBullet::AProjectileBullet()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->InitialSpeed = InitSpeed;
	ProjectileMovementComponent->MaxSpeed = InitSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = 0.1;
	
}


void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();

	// �Ѿ� ���� ǥ��  test
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
	ProjectilePath.OverrideGravityZ = 0.1f;
	ProjectilePath.ActorsToIgnore.Add(this);

	UGameplayStatics::PredictProjectilePath(this, ProjectilePath, ProjectileResult);

	DestoryProjectileStart();


}

// ����ü �ݸ��� ó��
void AProjectileBullet::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ĳ���Ͱ� ���� ������ ���� ����ü�� ������ ����ü�� �������� ó���� ���ε�
	// ���͸� ������ ���͸� �������� ���� GetOwner �Լ��̴� ���� ���������� �����;��� �����ʹ� character�� �������� ���̴�
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());

	class AWeapon* weapon = Cast<AWeapon>(GetOwner());

	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController)
		{
			UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
			
		}
	}

	Super::OnComponentHit(HitComp, OtherActor,  OtherComp, NormalImpulse, Hit);

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