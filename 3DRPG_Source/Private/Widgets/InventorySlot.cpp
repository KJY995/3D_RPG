// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventorySlot.h"
#include "Inventory/InventoryItem.h"
#include "Components/Button.h"
#include "Widgets/InventoryItemWidget.h"

bool UInventorySlot::Initialize()
{
	bool success = Super::Initialize();

	if (!success)
	{
		return false;
	}

	return true;
}

void UInventorySlot::OnCursor()
{

}

void UInventorySlot::OffCursor()
{

}

int32 UInventorySlot::AddItem(UInventoryItem* NewItem, int32 AddAmount)
{
	if (NewItem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventorySlot::AddItem Error : Try to Add Null ptr"));
	}

	if (IsEmptySlot())
	{
		ItemInSlot = NewItem;
		WBP_InventoryItem->AddItem(ItemInSlot);
	}

	Amount += AddAmount;
	return Amount;
}
int32 UInventorySlot::RemoveItem(int32 RemovingAmount)
{
	Amount -= RemovingAmount;

	if (Amount <= 0)
	{
		Amount = 0;
		ItemInSlot = nullptr;
	}

	return Amount;
}
bool UInventorySlot::IsEmptySlot()
{
	return (ItemInSlot == nullptr);
}
bool UInventorySlot::IsUsingSlot()
{
	return IsUsing;
}

void UInventorySlot::Refresh()
{
	WBP_InventoryItem->Refresh();
}