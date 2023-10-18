// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventorySlot;
class UInventoryItem;
class FReply;
class UButton;

UCLASS()
class OPENSLASH_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void PopUp();
	void PopOff();

	void AddItemToWidget(UInventoryItem* InventoryItem);

	virtual bool Initialize() override;

	void Refresh(int32 Number);
	void RefreshAll();

	UFUNCTION()
	void ExitOnClick();

private:
	UPROPERTY()
	TArray<UInventorySlot*> AllSlots;
	
	UPROPERTY(meta = (BindWidget))
	UButton* ExitButton;
};
