// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Shoot/Shoot.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/SkeletalMeshComponent.h"



AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	// test set fuc  �������� ������
	//SetReplicates(true);
	//SetReplicateMovement(true);

	//ScenRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//SetRootComponent(ScenRoot);  ������ ����

	//CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	//CollisionCapsule->SetupAttachment(RootComponent);
	//CollisionCapsule->SetCapsuleSize(1.f, 4.f, false);
	//CollisionCapsule->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	////CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//CollisionCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//CollisionCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	//CollisionCapsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

	//CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	//SetRootComponent(ScenRoot);
	//CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	////CollisionBox->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	//CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	//CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	//CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);
	CollisionSphere->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//CollisionSphere->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
	



}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// projectilecomponent�� ������ �ʰ� ������Ű�� ��ƼŬ�� ���� �ϰ� ������ �Ҹ��Ű�� �Լ��� ���
	if (ParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAttached
		(
			ParticleSystem,
			CollisionSphere,
			FName(),
			GetActorLocation(),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition
		);
	}
	
	SpawnTrailSystem();

	// �� Ŭ������ ��Ʈ��ũ ������ ������ �ִ��� ���θ� ��ȯ�Ѵ�
	if (HasAuthority())	// replicate = true
	{
		CollisionSphere->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
	}



}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}

void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	Destroy();

}

void AProjectile::Destroyed()
{
	Super::Destroyed();

	if (ImpactParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, GetActorTransform());
	}
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

}

void AProjectile::SpawnTrailSystem()
{
	if (TrailSystem)
	{
		TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(TrailSystem, GetRootComponent(), FName(""), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);

	}


}

void AProjectile::BlowupTimeToGrenadeStart()
{
	GetWorldTimerManager().SetTimer(DestroyTimeHandle, this, &AProjectile::BlowupTimeToGrenadeFinished, BlowupTimeToGrenade);

}

void AProjectile::BlowupTimeToGrenadeFinished()
{


	Destroy();
}

void AProjectile::ExplodeDamage()
{
	Character = Cast<ABaseCharacter>(GetOwner());

	APawn* Pawn = GetInstigator();   // ���� ������, �������� ����ߴ����� ������ �� �����´�  
	if (Pawn && HasAuthority())  // ���ۿ� ���� �������� �ƴ��� Ȯ���ϴ� if check
	{
		AController* controller = Pawn->GetController(); // �������� ��Ʈ�ѷ��� �����´�
		if (controller && Character)
		{
			UGameplayStatics::ApplyRadialDamageWithFalloff(
				this,
				GrenadeDamage,
				MinDamage,
				this->GetActorLocation(),
				DamageInnerRadius,
				DamageOuterRadius,
				1.f,
				UDamageType::StaticClass(),
				TArray<AActor*>(),
				this,
				controller);

			// addimpulse �� �߰��ؼ� ���߿� ��� ��� ���� �ʿ�

		}


	}
}