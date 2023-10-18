// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryItemWidget.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/InventoryComponent.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Character/MyCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint\WidgetBlueprintLibrary.h"
#include "Brushes/SlateImageBrush.h"

bool UInventoryItemWidget::Initialize()
{
	bool success = Super::Initialize();
	if (!success) return false;
	if (ItemBorder == nullptr) return false;

	if (UseButton == nullptr) return false;
	UseButton->OnClicked.AddDynamic(this, &UInventoryItemWidget::MakePlayerUsingItem);
	
	return true;
}

void UInventoryItemWidget::AddItem(UInventoryItem* InputItem)
{
	Item = InputItem;
	Thumbnail->SetBrushFromTexture(Item->Thumbnail);
}

void UInventoryItemWidget::MakePlayerUsingItem()
{
	if (Item == nullptr) return;

	AMyCharacter* player = Cast<AMyCharacter>(GetOwningPlayerPawn());
	if (player == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryItemWidget::MakePlayerUsingItem Error >>> GetOwningPlayerPawn() is nullptr"));
		return;
	}
	player->UseItem(Item);

	if (Item->IsEmpty())
	{
		RemoveItem();
	}
	return;
}

void UInventoryItemWidget::RemoveItem()
{
	Item = nullptr;
	FVector2d imageSize((double)64, (double)64);
}

void UInventoryItemWidget::Refresh()
{
	if (Item == nullptr) return;
	if (Item->IsEmpty())
	{
		RemoveItem();
	}
}