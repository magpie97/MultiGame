// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerSideRewindComponent.generated.h"

class ABaseCharacter;

USTRUCT(BlueprintType)
struct FHitBoxInfo
{
	GENERATED_BODY()

	// 위치
	UPROPERTY()
	FVector Location;
	
	// 회전
	UPROPERTY()
	FRotator Rotation;
	
	// 크기  (박스로 하니 크기 조절에 코드를 더 안 적어서 좋으듯 하다)
	UPROPERTY()
	FVector Extent;


};

USTRUCT(BlueprintType)
struct FServerSideRewindSnapshot
{
	GENERATED_BODY()

	// 리와인드 시간
	UPROPERTY()
	float Time;

	// 콜리전 박스 위치, 회전, 크기를 저장 할 컨테이너
	UPROPERTY()
	TMap<FName, FHitBoxInfo> HitBoxSnapshot;

};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	// 머리 확인
	UPROPERTY()
	bool bHitHead;

	// 몸통 확인
	//UPROPERTY()
	//bool bHitBody;

	// 맞았는지 체크
	UPROPERTY()
	bool bCheckHit;


	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOT_API UServerSideRewindComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UServerSideRewindComponent();
	
	// 캐릭터 클래스의 접근 권환을 완화 하여 사용하기 편하게 변경
	friend class ABaseCharacter;

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;





private:

	UPROPERTY()
	ABaseCharacter* Character;

	UPROPERTY()
	class AShooterPlayerController* PlayerController;

	// 프레임 당 캐릭터의 움직임의 시간을 저장할 컨테이너
	TDoubleLinkedList<FServerSideRewindSnapshot> FrameHistory;

	// framehistory의 최대 저장할 시간
	UPROPERTY(EditAnywhere)
	float MaxRewindTime = 3.f;

public:
	// 데미지 처리가 되었는지 확인 하기 위한 변수( info 저장된 구조체임)
	//FServerSideRewindSnapshot ServerSideRewindSnapshot;




public:

	// 게임 시작 후 캐릭터 콜리전 박스의 위치, 회전, 크기 값을 저장 할 함수
	UFUNCTION()
	void SaveHitBoxInfo(FServerSideRewindSnapshot& serversiderewindsnpshot);

	// 이 함수는 SaveServerSideRewind debug  전용
	UFUNCTION()
	void DebugSSR(FServerSideRewindSnapshot& serversiderewindsnpshot);

	// 저장된 캐릭터 info에 캐릭터가 hit 했을때 
	// hit 위치에 rewind를 진행 하여 3초 이전에 이벤트 발생시 데미지를 적용할지 말지 판단 할 함수
	UFUNCTION()
	FServerSideRewindResult ServerSideRewind(ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);

	// hit 했을 경우 콜리전을 rewind 하기 위한 함수
	UFUNCTION()
	FServerSideRewindResult CheckHitBody(FServerSideRewindSnapshot& SSRS, ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);
	
	// hit 위치 저장  save location
	UFUNCTION()
	void SaveTempBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS);

	// hit 위치로 박스 옮기기 set location
	UFUNCTION()
	void MoveTempBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS);

	// hit 위치로 옮기고 나서 다시 콜리전과 위치 초기화 진행에 필요한 함수
	UFUNCTION()
	void ResetBoxLocation(ABaseCharacter* HitCharacter, FServerSideRewindSnapshot& SSRS);

	// line trace에 방해가 될 메시 콜리전을 끄고 키기위한 함수
	UFUNCTION()
	void EnableChracterMeshCollision(ABaseCharacter* HitCharacter, ECollisionEnabled::Type Collision);

	// 서버에 먼저 데미지 처리
	UFUNCTION(Server, Reliable)
	void ServerDamageRequest(ABaseCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* Damage);

	// 스냅샷 info 저장 (call tick func)
	UFUNCTION()
	void SaveSnapshot();

};
