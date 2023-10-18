// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItemWeapon.h"
#include "Character/MyCharacter.h"
#include "ActorComponents/AttributeComponent.h"
#include "Inventory/InventoryComponent.h"
#include "Items/Weapons/Weapon.h"

UInventoryItemWeapon::UInventoryItemWeapon()
{
	SetCapacity(1);
	SetItemType(EItemType::EIT_Equippable);
}

void UInventoryItemWeapon::UseThisItem(AMyCharacter* MyCharacter)
{
	MyCharacter->EquipItem(this);
}