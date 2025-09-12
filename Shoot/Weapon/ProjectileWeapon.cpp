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


		// Ŭ���̾�Ʈ���� fire �Լ� ���� ����ü�� ������ �����Ǿ� ���ư��� ��Ȳ �߻�
		// ������ ���ؼ� ����Ǳ� ������ �̻��ϰ� ���̴°��� ���� (��Ŷ�ν� 400ms ��)
		// ������ ���ؼ� ������Ű�⺸�� Ŭ���̾�Ʈ���� �ﰢ �������� hit ������ �����ϴ°��� ���ƺ���
		
		AProjectile* SpawnProjectile = nullptr;

		if (bUseServerSideRewind)
		{
			// camera shake ��� �߰� �ʿ�  ���� �ĺ� Ŭ������ ��������


			if (InstigatorPawn->HasAuthority()) // ���������� ����
			{
				// ���� Ŭ�󿡼� inpact point�� �ι� ź���ϴ°����� ������ 
				if (InstigatorPawn->IsLocallyControlled()) //pawn���� ���÷� ���� �Է��� �޴��� Ȯ��
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);
					SpawnProjectile->bUseServerSideRewind = false;
					SpawnProjectile->Damage = Damage;

				}
				else
				{
					SpawnProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);
					SpawnProjectile->bUseServerSideRewind = true;

				}
			}
			else   // Ŭ���̾�Ʈ ssr ���
			{
				SpawnProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);
				SpawnProjectile->bUseServerSideRewind = true;
				SpawnProjectile->TraceStart = SocketTransform.GetLocation();
				SpawnProjectile->InitVelocity = SpawnProjectile->GetActorForwardVector() * SpawnProjectile->InitSpeed;
				SpawnProjectile->Damage = Damage;
			}
		}
		else
		{
			if (InstigatorPawn->HasAuthority()) // ����
			{
				SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);
				SpawnProjectile->bUseServerSideRewind = false;
				SpawnProjectile->Damage = Damage;

			}
		}
	}
}