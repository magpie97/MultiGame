// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOOT_API IMenuInterface
{
	GENERATED_BODY()



public:
	// 인터페이스 클래스엔 두 개의 클래스가 있고 맨 위에 있는 클래스는 건들이지 않는다
	// 아래의 클래스만 건들이며 완전히 구현하지 않는 함수가 있다면 오류를 발생한다 

	
	// 순수 가상 함수
	virtual void Host(FString ServerName) = 0;

	virtual void Join(uint32 Index) = 0;
	
	virtual void LoadMainMenu() = 0;
	
	virtual void RefreshServerList() = 0;



	


};
