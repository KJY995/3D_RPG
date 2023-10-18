// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/InventorySlot.h"
#include "Blueprint/UserWidget.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Border.h"

void UInventoryWidget::PopUp()
{
	AddToViewport();
}

void UInventoryWidget::PopOff()
{
	RemoveFromParent();
}

bool UInventoryWidget::Initialize()
{
	bool success = Super::Initialize();
	if (!success)
	{
		return false;
	}
	TArray<UUserWidget*> temp;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(GetWorld(), temp, UInventorySlot::StaticClass(), false);

	for (UUserWidget* slot : temp)
	{
		UInventorySlot* slotTemp = Cast<UInventorySlot>(slot);
		AllSlots.Add(slotTemp);
	}

	if (ExitButton != nullptr)
	{
		ExitButton->OnClicked.AddDynamic(this, &UInventoryWidget::ExitOnClick);
	}

	return true;
}

void UInventoryWidget::AddItemToWidget(UInventoryItem* InventoryItem)
{
	for (UInventorySlot* slot : AllSlots)
	{
		if (slot->IsEmptySlot())
		{
			slot->AddItem(InventoryItem, 1);
			return;
		}
	}
}

void UInventoryWidget::ExitOnClick()
{
	PopOff();
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	playerController->SetShowMouseCursor(false);
	FInputModeGameOnly gameOnly;
	playerController->SetInputMode(gameOnly);
}

void UInventoryWidget::Refresh(int32 Number)
{
	if (!AllSlots.IsValidIndex(Number))
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryWidget::Refresh Error >>> Try to access invalid tarray index"));
		return;
	}
	AllSlots[Number]->Refresh();
}

void UInventoryWidget::RefreshAll()
{
	for (UInventorySlot* slot : AllSlots)
	{
		slot->Refresh();
	}
}