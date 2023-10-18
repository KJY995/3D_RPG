// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryItem.h"
#include "InventoryItemWeapon.generated.h"

class AWeapon;

UCLASS()
class OPENSLASH_API UInventoryItemWeapon : public UInventoryItem
{
	GENERATED_BODY()

public:
	UInventoryItemWeapon();

	UPROPERTY(VisibleAnywhere)
	AWeapon* WeaponInstance;

protected:
	virtual void UseThisItem(class AMyCharacter* MyCharacter) override;
private:
};
