#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_ThrowGrenadeHold UMETA(DisplayName = "ThrowGrenadeHold"),
	ECS_ThrowGrenade UMETA(DisplayName = "ThrowGrenade"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX"),


};