// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShootGameMode.h"
#include "ShootCharacter.h"
#include "UObject/ConstructorHelpers.h"

/*
	�Ⱦ��� ���� �����
*/

AShootGameMode::AShootGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
