// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "InventoryItemFood.generated.h"

UCLASS()
class OPENSLASH_API UInventoryItemFood : public UInventoryItem
{
	GENERATED_BODY()
	
public:
	UInventoryItemFood();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (ClampMin = 0.0))
	float HealAmount = 1.f;

protected:
	virtual void UseThisItem(class AMyCharacter* MyCharacter) override;
};
