// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemFood.h"
#include "Character/MyCharacter.h"
#include "ActorComponents/AttributeComponent.h"
#include "Inventory/InventoryComponent.h"

UInventoryItemFood::UInventoryItemFood()
{
	SetItemType(EItemType::EIT_Consumable);
}

void UInventoryItemFood::UseThisItem(AMyCharacter* MyCharacter)
{
	if (MyCharacter)
	{
		MyCharacter->Healing(HealAmount);
		RemoveItemAmount(1);
	}
}