// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	//if (!HasAuthority()) return;	//���� üũ  �ӽ� �ּ�

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	UWorld* World = GetWorld();

	// �ش� ���� �����̸� ��������
	const USkeletalMeshSocket* MuzzleFlashSocketName = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	// ���� �̸��� true ���
	if (MuzzleFlashSocketName && World)
	{
		// �Ž� ���� �̸��� ���� ������ ��ġ��, ȸ����, ũ�Ⱚ�� �ش�Ž� weaponmesh �ζ��� �Լ��� ������ �����Ѵ�
		FTransform SocketTransform = MuzzleFlashSocketName->GetSocketTransform(GetWeaponMesh());
		// �ѱ����� ��ġ�� deprojectscreentoworld�Լ��� ���� ���� ���� �������� �Ÿ��� ��� 
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();


		// ������ ��������
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = GetOwner();
		ActorSpawnParameters.Instigator = InstigatorPawn;

		if (bUseServerSideRewind)
		{
			if (InstigatorPawn->HasAuthority()) // ������ �������� Ȯ��
			{
				if (InstigatorPawn->IsLocallyControlled()) //������ Ŭ���̾�Ʈ ���� Ȯ��
				{
					World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);


				}

			}


		}
		else
		{

		}





		World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);


	}

}