// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemData.generated.h"

class UInventoryItem;
class AItem;
class UWorld;

enum class EItemDataState : uint8
{
	EIDS_Field,
	EIDS_Inventory,
	EIDS_Equipped
};

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class OPENSLASH_API UItemData : public UObject
{
	GENERATED_BODY()

public:
	UItemData();

	UPROPERTY(EditAnywhere)
	float DisappearTime = 10.f;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryItem> InvItem;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AItem> FieldItem;

	//UFUNCTION()
	
	void DropItem(FVector3d Locaiton);

	void MoveToInventory();

	void Equipped();

	void Disappear();
private:
	UPROPERTY()
	UInventoryItem* InvItemInstance;

	UPROPERTY(EditAnywhere)
	AItem* FieldItemInstance;

	UPROPERTY()
	FTimerHandle DisappearTimer;

	void ChangeState(EItemDataState NextState);
};
