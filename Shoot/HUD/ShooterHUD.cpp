// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterHUD.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "PrepareToAttack.h"
#include "UObject/ConstructorHelpers.h"
#include "Shoot/PlayerController/ShooterPlayerController.h"
#include "Shoot/HUD/KillFeed.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/BackgroundBlur.h"
#include "Components/CanvasPanelSlot.h"


AShooterHUD::AShooterHUD()
{
	


}

void AShooterHUD::BeginPlay()
{
	Super::BeginPlay();


}

void AShooterHUD::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


	
}

bool AShooterHUD::IsNameStableForNetworking() const
{
	Super::IsNameStableForNetworking();

	return false;
}

bool AShooterHUD::IsSupportedForNetworking() const
{
	Super::IsSupportedForNetworking();

	return true;
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

void AShooterHUD::ShowKillFeedWidget(FString SuspectPlayerName, FString VictimPlayerName)
{
	PC = PC == nullptr ? GetOwningPlayerController() : PC;

	if (PC && KillFeedClass)
	{
		class UKillFeed* KillFeed = CreateWidget<UKillFeed>(PC, KillFeedClass);

		if (KillFeed)
		{
			KillFeed->ShowKillFeed(SuspectPlayerName, VictimPlayerName);

			KillFeed->AddToViewport();

			// 배열에 killfeed 위젯을 추가하여 겹치지 않게 표시함
			for (UKillFeed* ArrayKillFeed : StackKillFeed)
			{
				if (ArrayKillFeed && ArrayKillFeed->BG_Blur)
				{
					UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ArrayKillFeed->BG_Blur);

					if (CanvasPanelSlot)
					{
						FVector2D Position = CanvasPanelSlot->GetPosition();

						FVector2D NewPosition(CanvasPanelSlot->GetPosition().X, Position.Y + CanvasPanelSlot->GetSize().Y); // -
						
						CanvasPanelSlot->SetPosition(NewPosition);
						

					}
				}
			}

			StackKillFeed.Add(KillFeed);

			// 몇초 동안 표시할 Kill Feed Widget 
			FTimerHandle TimerHandle;
			FTimerDelegate TimerDelegate;
			
			KillFeed->ShowKillFeedWidgetAnimation();	// 애니메이션 보이기 

			TimerDelegate.BindUFunction(this, FName("DisappearKillFeedWidget"), KillFeed);  // default DisappearKillFeedWidget
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, KillFeedTimer, false);
			



		}
	}
}

void AShooterHUD::DisappearKillFeedWidget(UKillFeed* KillFeed)
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;

	if (KillFeed)
	{
		KillFeed->DisappearKillFeedWidgetAnimation();	// 애니메이션 사라지기

		TimerDelegate.BindUFunction(this, FName("DisappearKillFeedWidgetFinished"), KillFeed);
		GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, DisappearKillFeedTimer, false);

	}
}

void AShooterHUD::DisappearKillFeedWidgetFinished(UKillFeed* KillFeed)
{
	// 위젯을 지울때 텀을 두고 애니메이션 실행 후 지우는 함수 구현 필요
	
	if (KillFeed)
	{
		GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, TEXT("Removed KillFeed Widget"));

		KillFeed->RemoveFromParent();
		
		// 배열에 위젯이 삭제가 이루어 지지 않아 배열에 요소를 다시 삭제 하는 방법 선택
		StackKillFeed.Remove(KillFeed);

		
	}


}
