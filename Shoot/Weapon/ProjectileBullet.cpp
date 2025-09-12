// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/Weapon/Weapon.h"
#include "Shoot/ShootComponent/ServerSideRewindComponent.h" 

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"


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


	// 2�� �� ���� ����
	DestoryProjectileStart();


}

// ����ü �ݸ��� ó��
void AProjectileBullet::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ĳ���Ͱ� ���� ������ ���� ����ü�� ������ ����ü�� �������� ó���� ���ε�
	// ���͸� ������ ���͸� �������� ���� GetOwner �Լ��̴� ���� ���������� �����;��� �����ʹ� character�� �������� ���̴�
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());  //�ӽ� �ּ� ������� �ڵ�

	//ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());	// test

	if (OwnerCharacter)
	{
		AShooterPlayerController* OwnerController = Cast<AShooterPlayerController>(OwnerCharacter->Controller);
		if (OwnerController)
		{
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind) // ����, ssr�� �ƴѰ��
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());

				Super::OnComponentHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

				return;

				

			}

			ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(OtherActor);
			if (bUseServerSideRewind && OwnerCharacter->GetServerSideRewindComponent() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
			{
				OwnerCharacter->GetServerSideRewindComponent()->ServerProjectileDamageRequest(HitCharacter, TraceStart, InitVelocity, OwnerController->GetServerTime() - OwnerController->SingleTripTime);

				//debug
				//UE_LOG(LogTemp, Warning, TEXT("ApplyDamage"));

			}
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