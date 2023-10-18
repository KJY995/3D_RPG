// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryItem.h"
#include "Character/MyCharacter.h"

UInventoryItem::UInventoryItem()
{
	ItemDisplayName = FText::FromString("Item Insie Inventory");
	UseActionText = FText::FromString("I'm Used");
}

void UInventoryItem::UseThisItem(AMyCharacter* MyCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("This is Base Item"));
}

void UInventoryItem::SetCapacity(int32 Newcap)
{
	Capacity = Newcap;
}

bool UInventoryItem::AddItemAmount(int32 AddAmount)
{
	if (Capacity < AddAmount + Amount)
	{
		UE_LOG(LogTemp, Display, TEXT("Try to add too much item in one slot"));
		return false;
	}

	Amount += AddAmount;
	return true;
}

int32 UInventoryItem::RemoveItemAmount(int32 RemoveAmount)
{
	Amount -= RemoveAmount;
	if (Amount < 0)
	{
		Amount = 0;
	}

	return Amount;
}

int32 UInventoryItem::GetAffordCapacity()
{
	return Capacity - Amount;
}

int32 UInventoryItem::GetCapacity()
{
	return Capacity;
}