// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	//if (!HasAuthority()) return;	//권한 체크  임시 주석

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	// 해당 무기 소켓이름 가져오기
	const USkeletalMeshSocket* MuzzleFlashSocketName = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	// 소켓 이름이 true 라면
	if (MuzzleFlashSocketName)
	{
		// 매쉬 소켓 이름이 같은 소켓의 위치값, 회전값, 크기값을 해당매쉬 weaponmesh 인라인 함수로 가져와 저장한다
		FTransform SocketTransform = MuzzleFlashSocketName->GetSocketTransform(GetWeaponMesh());
		// 총구소켓 위치와 deprojectscreentoworld함수로 계산된 벡터 값을 뺀값으로 거리를 계산 
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.Owner = GetOwner();
			ActorSpawnParameters.Instigator = InstigatorPawn;

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);
			}

		}
		

	}

}