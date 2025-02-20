// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"


USTRUCT(BlueprintType)
struct FHUDCrosshairs
{
	GENERATED_BODY()

public:
	UPROPERTY()
	class UTexture2D* FHC_CrosshairCenter;

	UPROPERTY()
	UTexture2D* FHC_CrosshairLeft;
	
	UPROPERTY()
	UTexture2D* FHC_CrosshairRight;
	
	UPROPERTY()
	UTexture2D* FHC_CrosshairUp;
	
	UPROPERTY()
	UTexture2D* FHC_CrosshairDown;

	// 조준점 스프레드
	UPROPERTY()
	float CrosshairSpread;

};


UCLASS()
class SHOOT_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

private:
	AShooterHUD();



public:

	/*
		채팅 시스템
	*/
	TSharedPtr<class SWidget> GetChatInputTextObject();
	void AddChatMessage(const FString& Message);

	TSubclassOf<class UChatSystem> ChatSystemClass;
	class UChatSystem* ChatSystem;

	bool CheckUIObject();
	bool CreateUIObject();


public:

	FName GameState = "InProgress";

	// HUD class 내부에 선언된 함수 사용
	UFUNCTION()
	virtual void DrawHUD() override;

	// HUD에 player state를 표시할 클래스를 만든다
	UPROPERTY(EditAnywhere, Category = "PlayerState")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	// 캐릭터 hud 에 addtoviewport할 함수
	UFUNCTION()
	void AddCharacterOverlay();

	// HUD
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	// 게임 시작 준비 widget
	UPROPERTY(EditAnywhere, Category = "PrepareToAttackWidget")
	TSubclassOf<class UUserWidget> PrepareToAttackClass;

	// class 이름
	UPROPERTY()
	class UPrepareToAttack* PrepareToAttack;

	UFUNCTION()
	void AddPrepareToAttack();






protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;





private:
	UPROPERTY()
	float CrosshairResize;

private:
	UPROPERTY()
	FHUDCrosshairs HUDCrosshair;

	UFUNCTION()
	void DrawCrosshair(class UTexture2D* Texture, FVector2D ViewportSize);

public:

	// 잠시 가독성을 위해 줄을 바꿈
	FORCEINLINE void SetHUDCrosshairs(const FHUDCrosshairs& Crosshairs)
	{ 
		HUDCrosshair = Crosshairs;
	}



	
};
