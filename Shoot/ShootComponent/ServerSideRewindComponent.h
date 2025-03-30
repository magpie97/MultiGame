// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ServerSideRewindComponent.generated.h"

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

	UPROPERTY()
	class ABaseCharacter* Character;

	// 콜리전 박스 위치, 회전, 크기를 저장 할 컨테이너
	UPROPERTY()
	TMap<FName, FHitBoxInfo> HitBoxSnapshot;

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

	FServerSideRewindSnapshot ServerSideRewindSnapshot;


public:

	// 게임 시작 후 캐릭터 콜리전 박스의 위치, 회전, 크기 값을 저장 할 함수
	UFUNCTION()
	void SaveServerSideRewind(FServerSideRewindSnapshot& serversiderewindsnpshot);

	// 이 함수는 SaveServerSideRewind debug  전용
	UFUNCTION()
	void DebugSSR(FServerSideRewindSnapshot& serversiderewindsnpshot);

	


		
};
