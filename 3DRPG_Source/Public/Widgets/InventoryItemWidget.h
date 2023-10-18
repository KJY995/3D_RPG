// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

class UInventoryItem;
class UImage;
class UButton;
class UBorder;
class UTextBlock;

UCLASS()
class OPENSLASH_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddItem(UInventoryItem* InputItem);
	void Refresh();

	UFUNCTION()
	void MakePlayerUsingItem();

	UFUNCTION()
	void RemoveItem();

protected:
	virtual bool Initialize();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (AllowPrivateAccess = true))
	UInventoryItem* Item;

	UPROPERTY(meta = (BindWidget))
	UImage* Thumbnail;

	UPROPERTY(meta = (BindWidget))
	UBorder* ItemBorder;

	UPROPERTY(meta = (BindWidget))
	UButton* UseButton;
};
