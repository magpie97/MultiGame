// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatSystem.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Shoot/HUD/Chat.h"

TSharedPtr<class SWidget> UChatSystem::GetChatInputTextObject()
{
	return WBP_Chat->GetChatInputTextObject();
}

void UChatSystem::AddChatMessage(const FString& message)
{
	WBP_Chat->AddChatMessage(*message);

}
