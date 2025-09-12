// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"


void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	//if (!HasAuthority()) return;	//권한 체크  임시 주석

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	UWorld* World = GetWorld();

	// 해당 무기 소켓이름 가져오기
	const USkeletalMeshSocket* MuzzleFlashSocketName = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

	// 소켓 이름이 true 라면
	if (MuzzleFlashSocketName && World)
	{
		// 매쉬 소켓 이름이 같은 소켓의 위치값, 회전값, 크기값을 해당매쉬 weaponmesh 인라인 함수로 가져와 저장한다
		FTransform SocketTransform = MuzzleFlashSocketName->GetSocketTransform(GetWeaponMesh());
		// 총구소켓 위치와 deprojectscreentoworld함수로 계산된 벡터 값을 뺀값으로 거리를 계산 
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();


		// 포인터 가져오기
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.Owner = GetOwner();
		ActorSpawnParameters.Instigator = InstigatorPawn;


		// 클라이언트에서 fire 함수 실행 투사체가 느리게 스폰되어 날아가는 상황 발생
		// 서버를 통해서 복사되기 때문에 이상하게 보이는것이 정상 (패킷로스 400ms 임)
		// 서버를 통해서 스폰시키기보단 클라이언트에서 즉각 스폰시켜 hit 판정을 조정하는것이 좋아보임
		
		AProjectile* SpawnProjectile = nullptr;

		if (bUseServerSideRewind)
		{
			// camera shake 기능 추가 필요  현제 컴벳 클래스에 적용했음


			if (InstigatorPawn->HasAuthority()) // 서버에서만 실행
			{
				// 현재 클라에선 inpact point에 두번 탄착하는것으로 보여짐 
				if (InstigatorPawn->IsLocallyControlled()) //pawn으로 로컬로 부터 입력을 받는지 확인
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
			else   // 클라이언트 ssr 사용
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
			if (InstigatorPawn->HasAuthority()) // 서버
			{
				SpawnProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, ActorSpawnParameters);
				SpawnProjectile->bUseServerSideRewind = false;
				SpawnProjectile->Damage = Damage;

			}
		}
	}
}