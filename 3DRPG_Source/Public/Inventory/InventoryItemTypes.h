#pragma once

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Base UMETA(DisplayName = "Base"),
	EIT_Consumable UMETA(DisplayName = "Consumable"),
	EIT_Equippable UMETA(DisplayName = "Equippable")
};