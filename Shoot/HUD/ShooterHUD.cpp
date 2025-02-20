// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "PrepareToAttack.h"
#include "Shoot/HUD/Chat.h"
#include "Shoot/HUD/ChatSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"


AShooterHUD::AShooterHUD()
{
	// WidgetBlueprint'/Game/Blueprint/HUD/WBP_ChatSystem.WBP_ChatSystem'
	static ConstructorHelpers::FClassFinder<UChatSystem> ChatSystemWBPClass(TEXT("/Game/Blueprint/HUD/WBP_ChatSystem"));
	
	if (ChatSystemWBPClass.Succeeded())
	{
		ChatSystemClass = ChatSystemWBPClass.Class;
	}
	


}


void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();


	CheckUIObject();



}

void AShooterHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	
}

// 캐릭터 상태 widget
void AShooterHUD::AddCharacterOverlay()
{
	// 이 HUD의 플레이어에 대한 PlayerController를 리턴하는 함수
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);

		CharacterOverlay->AddToViewport();

		
		
	}
}

// 게임 시작전 카운팅을 위한 Widget
void AShooterHUD::AddPrepareToAttack()
{
	// 이 HUD의 플레이어에 대한 PlayerController를 리턴하는 함수
	APlayerController* PlayerController = GetOwningPlayerController();

	if (PlayerController && PrepareToAttackClass)
	{
		PrepareToAttack = CreateWidget<UPrepareToAttack>(PlayerController, PrepareToAttackClass);

		PrepareToAttack->AddToViewport();
		
	}
}

void AShooterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewPortSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
		const FVector2D ViewPortCanterPosition(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);

		if (HUDCrosshair.FHC_CrosshairCenter)
		{
			DrawCrosshair(HUDCrosshair.FHC_CrosshairCenter, ViewPortCanterPosition);
		}
		if (HUDCrosshair.FHC_CrosshairLeft)
		{
			DrawCrosshair(HUDCrosshair.FHC_CrosshairLeft, ViewPortCanterPosition);
		}
		if (HUDCrosshair.FHC_CrosshairRight)
		{
			DrawCrosshair(HUDCrosshair.FHC_CrosshairRight, ViewPortCanterPosition);
		}
		if (HUDCrosshair.FHC_CrosshairUp)
		{
			DrawCrosshair(HUDCrosshair.FHC_CrosshairUp, ViewPortCanterPosition);
		}
		if (HUDCrosshair.FHC_CrosshairDown)
		{
			DrawCrosshair(HUDCrosshair.FHC_CrosshairDown, ViewPortCanterPosition);
		}
	}
}



void AShooterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportSize)
{
	// crosshair 사이즈 변경
	CrosshairResize = ViewportSize.X / 3840;

	// texture2D 의 가로 세로 값을 가져오는 함수를 사용하여 변수 초기화 후 
	const float TextureWidth = Texture->GetSizeX() * CrosshairResize;
	const float TextureHeight = Texture->GetSizeY() * CrosshairResize;





	// 2 씩 나눠 절반의 값을 각각 위치값을 저장 
	const FVector2D TextureDrawPoint(ViewportSize.X - (TextureWidth / 2.f), ViewportSize.Y - (TextureHeight / 2.f));

	DrawTexture(Texture, TextureDrawPoint.X, TextureDrawPoint.Y, TextureWidth, TextureHeight, 0.f, 0.f, 1.f, 1.f, FLinearColor::Green);

}

TSharedPtr<class SWidget> AShooterHUD::GetChatInputTextObject()
{

	return ChatSystem->GetChatInputTextObject();
}

void AShooterHUD::AddChatMessage(const FString& Message)
{
	if (!CheckUIObject()) return;

	ChatSystem->AddChatMessage(*Message);
}

bool AShooterHUD::CheckUIObject()
{
	// 원래 사용하던 코드
	if (ChatSystem == nullptr)
	{
		return CreateUIObject();
	}

	return true;
}

bool AShooterHUD::CreateUIObject()
{
	if (ChatSystemClass)
	{
		ChatSystem = CreateWidget<UChatSystem>(GetOwningPlayerController(), ChatSystemClass);
		if (ChatSystem)
		{
			ChatSystem->AddToViewport();
			return true;
		}
	}

	return false;
}