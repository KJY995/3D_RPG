// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UInventoryItem;
class UInventorySlot;
class UInventoryWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENSLASH_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	virtual void BeginPlay() override;

	void PopInventoryWidget();
	void OffInventoryWidget();
	bool AddItem(UInventoryItem* InventoryItem, int32 Amount = 1);
	bool RemoveItem(UInventoryItem* InventoryItem);

	bool IsWidgetInViewport();

	UInventoryItem* GetSameItem(UInventoryItem* InventoryItem);

	void Refresh();
private:
	bool AddItemLoop(UInventoryItem* InventoryItem, int32& Amount);

	UPROPERTY(EditDefaultsOnly, Instanced)
	TArray<UInventoryItem*> DefaultItems;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int32 Capacity = 40;

	UPROPERTY()
	UInventoryWidget* InventoryWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<UInventoryWidget> InventoryWidget;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryUpdated OnInventoryUpdated;

	UPROPERTY(VisibleAnyWhere, Category = "Items")
	TArray<UInventoryItem*> Items;
};
