// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemTypes.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItem.generated.h"

enum class EInvItemDState : uint8
{
	EIIS_Inventory,
	EIIS_Equipped
};

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew, DefaultToInstanced)
class OPENSLASH_API UInventoryItem : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItem();

	virtual class UWorld* GetWorld() const { return World; }

	UPROPERTY(Transient)
	class UWorld* World;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText UseActionText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	class UStaticMesh* PickupMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	class UTexture2D* Thumbnail;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	FText ItemDisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item", meta = (MultiLine = true))
	FText ItemDescription;

	UPROPERTY()
	class UInventoryComponent* OwningInventory;

	// AddAmount 만큼 더한다, 만약 덧셈의 결과로 Capacity 를 초과하게 된다면 덧셈 작업을 하지 않고 false를 반환한다.
	UFUNCTION()
	bool AddItemAmount(int32 AddAmount);
	
	// Amount 만큼 빼고, 남은 양을 반환한다.
	UFUNCTION()
	int32 RemoveItemAmount(int32 RemoveAmount);

	virtual void UseThisItem(class AMyCharacter* MyCharacter);
	
	UFUNCTION()
	void SetCapacity(int32 Newcap);

	UFUNCTION()
	int32 GetCapacity();

	UFUNCTION()
	int32 GetAffordCapacity();

private:
	UPROPERTY(EditDefaultsOnly)
	int32 ItemID = 0;

	UPROPERTY(VisibleAnywhere, Category = "Item")
	int32 Amount = 1;
	
	UPROPERTY(EditAnywhere)
	int32 Capacity = 40;

	UPROPERTY(VisibleAnywhere)
	EItemType ItemType = EItemType::EIT_Base;

public:
	inline EItemType GetItemType() { return ItemType; }
	inline void SetItemType(EItemType NewItemType) { ItemType = NewItemType; }
	inline void SetItemID(int32 NewID) { ItemID = NewID; }
	inline int32 GetItemID() { return ItemID; }
	inline bool IsEmpty() { return (Amount <= 0); }
};