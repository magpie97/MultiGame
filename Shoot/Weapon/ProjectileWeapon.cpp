// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	//if (!HasAuthority()) return;	//���� üũ  �ӽ� �ּ�

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());

	// �ش� ���� �����̸� ��������
	const USkeletalMeshSocket* MuzzleFlashSocketName = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	// ���� �̸��� true ���
	if (MuzzleFlashSocketName)
	{
		// �Ž� ���� �̸��� ���� ������ ��ġ��, ȸ����, ũ�Ⱚ�� �ش�Ž� weaponmesh �ζ��� �Լ��� ������ �����Ѵ�
		FTransform SocketTransform = MuzzleFlashSocketName->GetSocketTransform(GetWeaponMesh());
		// �ѱ����� ��ġ�� deprojectscreentoworld�Լ��� ���� ���� ���� �������� �Ÿ��� ��� 
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