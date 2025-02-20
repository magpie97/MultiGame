// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class SHOOT_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	


public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	UFUNCTION()
	void SetDisplayText(FString TextToDisplay);

	// test func
	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

	UPROPERTY()
	class ABaseCharacter* BaseCharacter;


	// test
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* UserNameWidget;

	UPROPERTY()
	class UOverheadWidget* OverheadWidget;

	//UFUNCTION(BlueprintCallable)
	//void ShowUserNameWidget(APawn* InPawn);

	UPROPERTY()
	class AShooterPlayerController* PlayerController;

	UPROPERTY()
	class AShooterPlayerState* PlayerState;

	
	



protected:
	UFUNCTION()
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld) override;

};
