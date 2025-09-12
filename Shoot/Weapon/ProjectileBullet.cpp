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


	// 2초 후 액터 삭제
	DestoryProjectileStart();


}

// 투사체 콜리전 처리
void AProjectileBullet::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 캐릭터가 총을 가지고 총은 투사체를 가지고 투사체의 데미지를 처리할 것인데
	// 액터를 소유한 액터를 가져오는 것이 GetOwner 함수이다 따라서 최종적으로 가져와야할 포인터는 character의 포인터인 것이다
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());  //임시 주석 사용중인 코드

	//ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());	// test

	if (OwnerCharacter)
	{
		AShooterPlayerController* OwnerController = Cast<AShooterPlayerController>(OwnerCharacter->Controller);
		if (OwnerController)
		{
			if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind) // 서버, ssr가 아닌경우
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