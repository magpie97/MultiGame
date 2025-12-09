// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatBox.generated.h"

/**
 * 
 */

class UScrollBox;
class UEditableText;
class AShooterPlayerController;

UCLASS()
class SHOOT_API UChatBox : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	// 스크롤 박스  채팅의 기록을 담을 변수
	UPROPERTY()
	UScrollBox* ChatHistory_ScrollBox;

	// 채팅에 필요한 문장을 입력받을 변수
	UPROPERTY()
	UEditableText* InputText_EditableText;

	// 포커스를 할지 안할지 결정하는 변수
	UPROPERTY()
	bool ChatFocus = false;

	UPROPERTY()
	float ScrollStrength = 60.f;

	UPROPERTY(EditAnywhere, Category = "Chat")
	TSubclassOf<class UChatPlayerController> ChatPlayerControllerClass;

	// 액터 컴포넌트임
	UPROPERTY()
	class UChatPlayerController* ChatPlayerControllerRef;

	UPROPERTY()
	class UChatEntry* ChatEntry;

	UPROPERTY(EditAnywhere, Category = "Chat")
	TSubclassOf<class UChatEntry> ChatEntryClass;


	// 굳이 설명이 필요없는 주석 처리는 완성 이후에 정리 필요
public:
	
	// enter 키를 누르면 위젯의 EditableText에 포커스 되어 채팅이 가능하게 하는 함수
	UFUNCTION()
	void FocusChat();

	// 입력 후 enter를 누르면 위젯의 Focus 를 끄고 다시 Gameonly로 변경하여 게임 진행 하는 함수  
	UFUNCTION()
	void ExitChat();

	// 채팅 기록을 보기 위한 마우스 휠 기능
	UFUNCTION()
	void Scroll(bool bUpScroll);

	UFUNCTION()
	void AddChatMessage(FString message);
	
};
