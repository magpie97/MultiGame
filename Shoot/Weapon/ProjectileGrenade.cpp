// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Shoot/Character/BaseCharacter.h"
#include "Components/SkeletalMeshComponent.h"


AProjectileGrenade::AProjectileGrenade()
{
	GrenadeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMeshComponent"));
	GrenadeMeshComponent->SetupAttachment(RootComponent);
	GrenadeMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;




}

void AProjectileGrenade::BeginPlay()
{
	AActor::BeginPlay();

	// test
	/*FPredictProjectilePathParams ProjectilePath;
	FPredictProjectilePathResult ProjectileResult;
	ProjectilePath.bTraceWithChannel = true;
	ProjectilePath.bTraceWithCollision = true;
	ProjectilePath.DrawDebugTime = 3.f;
	ProjectilePath.DrawDebugType = EDrawDebugTrace::ForDuration;
	ProjectilePath.LaunchVelocity = GetActorForwardVector() * InitSpeed;
	ProjectilePath.MaxSimTime = 7.f;
	ProjectilePath.ProjectileRadius = 2.f;
	ProjectilePath.SimFrequency = 10.f;
	ProjectilePath.StartLocation = GetActorLocation();
	ProjectilePath.TraceChannel = ECollisionChannel::ECC_Visibility;
	ProjectilePath.OverrideGravityZ = 1.f;
	ProjectilePath.ActorsToIgnore.Add(this);*/

	// test
	//ProjectilePath.bTraceComplex = true;

	//UGameplayStatics::PredictProjectilePath(this, ProjectilePath, ProjectileResult);


	SpawnTrailSystem();

	DestoryProjectileStart();


	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);
	
	
	

}

void AProjectileGrenade::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}



void AProjectileGrenade::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//if (OtherActor == GetOwner()) return;


}

void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	// 수류탄 팅기는 소리

	if (BounceSound)
	{
		FVector Velocity = this->GetVelocity();
		Speed = Velocity.Size();
		if (Speed > MaxSpeed) // 0 > 300.f
		{
			UGameplayStatics::PlaySoundAtLocation(this, BounceSound, GetActorLocation());


		}
	}
}


void AProjectileGrenade::Destroyed()
{


	ExplodeDamage();

	// todo : addimpulse 를 추가하여 폭탄이 터질때 밀어내기 기능 구현 필요

	//GrenadeMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	//GrenadeMeshComponent->AddImpulse(FVector(5000.f, 5000.f, 5000.f));
	//GrenadeMeshComponent->AddImpulseAtLocation(FVector(0.f, 0.f, 50000.f), GetActorLocation()); // test
	//Character->GetMesh()->SetSimulatePhysics(true);
	//GrenadeMeshComponent->AddRadialForce(GetActorLocation(), 50000.f, 50000.f, ERadialImpulseFalloff::RIF_Constant);

	/*if (Character == nullptr) return;
	if (Character)
	{
		Character->GetMesh()->AddRadialForce(GetActorLocation(), 400.f, 10000.f, ERadialImpulseFalloff::RIF_Constant);
	}*/
	

	//UE_LOG(LogTemp, Warning, TEXT("GrenadeLocation : %s"), *GetActorLocation().ToString());

	
	
	Super::Destroyed();

}
