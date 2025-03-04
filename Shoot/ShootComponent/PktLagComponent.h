// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PktLagComponent.generated.h"


// ���� ������Ʈ�� PktLag collision�� ������ ������ ����ü
USTRUCT(BlueprintType)
struct FPktLagCapsuleTransform
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

// ĳ���� PktLag ����ü, 
USTRUCT(BlueprintType)
struct FPktLagStruct
{
	GENERATED_BODY()

	float PktLagTime;

	TMap<FName, FPktLagCapsuleTransform> PktLagCapsuleMap;


};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOOT_API UPktLagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPktLagComponent();

	friend class ABaseCharacter;

	//PktLag ������Ʈ�� transform�� ����
	UFUNCTION()
	void PktLagSaveLocation(FPktLagStruct& PktLagStruct);

	// PktLag ����� ����
	UFUNCTION()
	void PktLagDebugCapsule(FPktLagStruct& PktLagStruct, FColor Color);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	class ABaseCharacter* Character;

	UPROPERTY()
	class AShooterPlayerController* PlayerController;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
