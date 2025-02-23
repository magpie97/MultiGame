#pragma once

UENUM(BlueprintType)
enum class EWeaponTypes : uint8
{
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),
	EWT_Grenade UMETA(DisplayName = "Grenade"),

	EWT_MAX UMETA(DisplayName = "MAX"),


};
