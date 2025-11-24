// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ShootGameInstance.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FTestDelegateSignature, int32 number);

UCLASS()
class SHOOT_API UShootGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:
	UShootGameInstance(const FObjectInitializer& ObjectInitializer);

	// gameinstance가 필요한 것을 설정할 수 있는 기회를 허용하는 가상 함수  필수
	virtual void Init();

	// callable 해서 블루프린트로 함수를 사용가능
	UFUNCTION(BlueprintCallable)
	void LoadMenuWidget();

	UFUNCTION(BlueprintCallable)
	void InGameLoadMenuWidget();

	UFUNCTION(Exec)
	void Host(FString ServerName) override;

	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	virtual void LoadMainMenu() override;

	virtual void RefreshServerList() override;


private:
	// FClassFinder 변수 문법 https://docs.unrealengine.com/4.27/en-US/API/Runtime/CoreUObject/UObject/ConstructorHelpers/FClassFinder/
	TSubclassOf<class UUserWidget> MenuClass;		// createwidget 을 쓸때 TSubclassOf를 꼭사용해야 한다 

	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UMainMenu* Menu;

	// typedef TSharedPtr<IOnlineSession, ESPMode::ThreadSafe> IOnlineSessionPtr로 정의 되어 있음
	IOnlineSessionPtr SessionInterface;
	// 스마트 포인터 <type> FOnlineSessionSearch는 class 이다 
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	//UPROPERTY()
	//FString UserName;

	/*UPROPERTY()
	class AShooterPlayerController* ShooterPlayerController;

	UPROPERTY()
	class AShooterPlayerState* PlayerState;*/

	

public:
	// 채팅시스템에서 사용할 함수
	UFUNCTION()
	FString GetUserName();



	void OnCreateSessionComplete(FName SessionName, bool Successful);
	void OnDestroySessionComplete(FName SessionName, bool Successful);
	void OnFindSessionsComplete(bool Successful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);





public:
	// 원하는 서버 이름
	FString DesiredServerName;

	// 세션을 만들었을때 메뉴 위젯의 메뉴 설정 섹션에 몇 가지 입력을 (게임 인원 수 등등..) 추가가 필요
	// 공개 세션에 플레이어의 연결할 수 2명 4명 등등
	int32 NumPublicConnections{ 4 }; // 최소 4명 부터 defualt 4
	FString MatchType{ TEXT("FreeForAll") }; // 옵션

	// int32 NumPublicConnections, FString MatchType
	void CreateSession();


public:
	// 로비에 사용할 func
	FORCEINLINE int32 GetNumPublicConnections() const { return NumPublicConnections; }

	// 채팅시스템에서 사용할 함수
	//FORCEINLINE FString GetUserName() const { return UserName; }









};
