// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/InventoryWidget.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/InventoryWidget.h"
#include "Widgets/InventorySlot.h"
#include "Inventory/InventoryItemTypes.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{	
	InventoryWidgetInstance = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidget);
	for (UInventoryItem* item : DefaultItems)
	{
		AddItem(item, 1);
	}
}

bool UInventoryComponent::IsWidgetInViewport()
{
	if (InventoryWidgetInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::IsWidgetInViewport Error >>> InventoryWidgetInstance is nullptr"));
		return false;
	}
	return InventoryWidgetInstance->IsInViewport();
}

// Amount 가 0인 아이템들을 전부 nullptr 로 바꿔준다.
// 이 후 위젯 쪽으로 내려가서 Refresh 를 전부 돌려준다.
void UInventoryComponent::Refresh()
{
	for (UInventoryItem* item : Items)
	{
		if (item->IsEmpty())
		{
			RemoveItem(item);
		}
	}
}

bool UInventoryComponent::AddItem(UInventoryItem* InventoryItem, int32 Amount)
{
	if (!InventoryItem)
	{
		UE_LOG(LogTemp, Error, TEXT("UINVENTORY COMPONENT -> ADDITEM ERROR: TRY TO ADD NULL PTR TO INVENTORY"));
		return false;
	}
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("UINVENTORY COMPONENT -> ADDITEM ERROR: Try to add negative or 0 amount of item"));
		return false;
	}
	InventoryItem->World = GetWorld();

	if (!AddItemLoop(InventoryItem, Amount))
	{
		UE_LOG(LogTemp, Display, TEXT("UINVENTORY COMPONENT -> ADDITEM ERROR: Try to add too much item"));
	}

	OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::AddItemLoop(UInventoryItem* InventoryItem, int32& Amount)
{	
	// Inventory Item 이 nullptr 인 지 체크한다.
	if (InventoryItem == nullptr) return false;

	// nullptr이 아니라면 루프를 시작한다.
	// 루프를 반복할 때마다, Amount의 개수를 체크해준다.
	// 0이 되었다면 루프에서 빠져나오며 true 를 반환한다.
	// 만약 0이 되기 전에 인벤토리가 가득 찼다면, 아래 쪽에서 false 를 반환하고 종료된다.
	while (Amount > 0)
	{
		//---------루프 시작---------------

		// 우선 인벤토리에 추가해주려는 아이템과 같은 종류의 아이템이 이미 있는지 체크한다.
		// 그 아이템의 현재 캐퍼시티가 여유가 있는 지는 GetSameItem 내부에서 자동으로 체크된다.
		UInventoryItem* item = GetSameItem(InventoryItem);

		// 이미 인벤토리 안에 같은 아이템이 있고, 그 아이템의 캐퍼시티가 여유가 있다면,
		// // 꽉 차기 전까지 Amount 만큼 아이템을 넣어준다.
		if (item != nullptr)
		{
			int32 afforadbleSpace = item->GetAffordCapacity();
			if (afforadbleSpace >= Amount)
			{
				item->AddItemAmount(Amount);
				Amount = 0;
			}
			else
			{
				item->AddItemAmount(afforadbleSpace);
				Amount -= afforadbleSpace;
			}
		}
		// 그러한 아이템이 인벤토리에 없다면,
		// Inventory 자체의 Capacity를 체크해준다.
		// 만약 Inventory 자체의 Capacity가 여유가 있다면,
		// //  TArray Items 에다가 새로운 아이템을 딱 한 개 넣어주고, Amount 를 1 줄여준다.
		else
		{
			if (Items.Num() >= Capacity)
			{
				UE_LOG(LogTemp, Warning, TEXT("Too much Item inside Inventory"));
				return false;
			}
			else
			{
				Items.Add(InventoryItem);
				InventoryWidgetInstance->AddItemToWidget(InventoryItem);
				Amount--;
			}
		}
	}
	return true;
}

bool UInventoryComponent::RemoveItem(UInventoryItem* InventoryItem)
{
	if (!InventoryItem)
	{
		UE_LOG(LogTemp, Error, TEXT("UINVENTORY COMPONENT -> REMOVEITEM ERROR: TRY TO REMOVE NULL PTR FROM INVENTORY"));
		return false;
	}

	InventoryItem->OwningInventory = nullptr;
	InventoryItem->World = nullptr;
	Items.RemoveSingle(InventoryItem);

	OnInventoryUpdated.Broadcast();

	return true;
}

UInventoryItem* UInventoryComponent::GetSameItem(UInventoryItem* InventoryItem)
{
	for (UInventoryItem* item : Items)
	{
		if (item->GetItemID() == InventoryItem->GetItemID())
		{
			if (item->GetAffordCapacity() > 0)
			{
				return item;
			}
		}
	}

	return nullptr;
}

void UInventoryComponent::PopInventoryWidget()
{
	if (InventoryWidgetInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::PopInventoryWidget Error: InventoryWidgetInstance is nullptr"));
		return;
	}
	
	InventoryWidgetInstance->PopUp();
}

void UInventoryComponent::OffInventoryWidget()
{
	if (InventoryWidgetInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::OffInventoryWidget Error: InventoryWidgetInstance is nullptr"));
		return;
	}

	InventoryWidgetInstance->PopOff();
}