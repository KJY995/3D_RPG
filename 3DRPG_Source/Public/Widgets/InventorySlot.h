// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlot.generated.h"

class UInventoryItemWidget;
class UInventoryItem;
class UButton;
class UImage;
class UTextBlock;

UCLASS()
class OPENSLASH_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void OnCursor();
	void OffCursor();

	int32 AddItem(UInventoryItem* NewItem, int32 AddAmount = 1);
	int32 RemoveItem(int32 RemovingAmount);
	bool IsEmptySlot();
	bool IsUsingSlot();

	void Refresh();

protected:
	virtual bool Initialize();

private:	
	UPROPERTY(meta = (BindWidget))
	UInventoryItemWidget* WBP_InventoryItem;

	UPROPERTY()
	UInventoryItem* ItemInSlot;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmountText;
	
	int32 Amount = 0;
	bool IsUsing = false;
};
