// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PktLagComponent.generated.h"


// 액터 컴포넌트인 PktLag collision의 정보를 저장할 구조체
USTRUCT(BlueprintType)
struct FPktLagCapsuleInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FVector PktLagCapsuleComponentLocation;

	UPROPERTY()
	FRotator PktLagCapsuleComponentRotator;

	UPROPERTY()
	float PktLagCapsuleComponentRadius;

	UPROPERTY()
	float PktLagCapsuleComponentHalfHeight;


};

// 캐릭터 PktLag 구조체
USTRUCT(BlueprintType)
struct FPktLagCapsuleTransformMap
{
	GENERATED_BODY()

	// 게임 시작 GetTimeSecond 함수로 레벨이 생성된 시간을 가져와서 초기화 했음
	float PktLagServerTime;

	// 캡슐 위치, 회전, 크기 값을 저장할 컨테이너
	TMap<FName, FPktLagCapsuleInfo> PktLagCapsuleInfo;


};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOT_API UPktLagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPktLagComponent();

	friend class ABaseCharacter;

	//PktLag 컴포넌트에 transform을 저장
	UFUNCTION()
	void PktLagSaveLocation(FPktLagCapsuleTransformMap& PktLagStructMap);

	// PktLag 디버그 전용
	UFUNCTION()
	void PktLagDebugCapsule(FPktLagCapsuleTransformMap& PktLagStructMap, FColor Color);





protected:
	// Called when the game starts
	virtual void BeginPlay() override;






private:

	UPROPERTY()
	class ABaseCharacter* Character;

	UPROPERTY()
	class AShooterPlayerController* PlayerController;

	// 게임 시작 후 위치 저장, 서버 지연시간에 비례하여 해당 시간에 머물렀던 액터의 위치를 저장 그 후 오래된 위치값을 삭제 ( defualt 4초로 설정)
	// 위 작업을 하기 위해 필요한 컨테이너
	TDoubleLinkedList<FPktLagCapsuleTransformMap> PktLagLocationHistoryLinkedList;

	// 서버 지연 시간을 임의로 지정하여 테스트 하기 위한 변수  4초로 테스트
	// 4초
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;





public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
