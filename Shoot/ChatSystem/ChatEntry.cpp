// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatEntry.h"
#include "Components/TextBlock.h"

void UChatEntry::NativeConstruct()
{
	SetText();

}

void UChatEntry::SetText()
{
	TextMessage_TextBlock->SetText(FText::FromString(TextMessage));


}
