// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemData.h"
#include "Inventory/InventoryItem.h"
#include "Items/Item.h"

UItemData::UItemData()
{

}

void UItemData::ChangeState(EItemDataState NextState)
{
	switch (NextState)
	{
	case EItemDataState::EIDS_Field:
		if (FieldItem == nullptr || InvItemInstance == nullptr) return;
		InvItemInstance->ConditionalBeginDestroy();
		if (GetWorld() != nullptr)
		{
			GetWorld()->GetTimerManager().SetTimer(DisappearTimer, this, &UItemData::Disappear, DisappearTime);
		}
		return;

	case EItemDataState::EIDS_Inventory:
		if (InvItem == nullptr || FieldItemInstance == nullptr) return;
		FieldItemInstance->Destroy();
		if (GetWorld() != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(DisappearTimer);
		}
		return;

	case EItemDataState::EIDS_Equipped:
		if (InvItem == nullptr || FieldItem == nullptr) return;
		return;
	
	default:
		UE_LOG(LogTemp, Warning, TEXT("ItemData->ChangeState Error: No Such State Exist."));
		return;
	}
}

void UItemData::DropItem(FVector3d Location)
{
	if (GetWorld() != nullptr)
	{
		FieldItemInstance = GetWorld()->SpawnActor<AItem>(FieldItem);
		FieldItemInstance->SetActorLocation(Location);
	}
	ChangeState(EItemDataState::EIDS_Field);
}

void UItemData::MoveToInventory()
{
	InvItemInstance = NewObject<UInventoryItem>(InvItem);
	ChangeState(EItemDataState::EIDS_Inventory);
}

void UItemData::Equipped()
{
	if (GetWorld() != nullptr)
	{
		FieldItemInstance = GetWorld()->SpawnActor<AItem>(FieldItem);
	}
	ChangeState(EItemDataState::EIDS_Equipped);
}

void UItemData::Disappear()
{
	GetWorld()->GetTimerManager().ClearTimer(DisappearTimer);
	if (FieldItemInstance != nullptr)
	{
		FieldItemInstance->Destroy();
	}
	if (InvItemInstance != nullptr)
	{
		InvItemInstance->ConditionalBeginDestroy();
	}
	RemoveFromRoot();
	ConditionalBeginDestroy();
}