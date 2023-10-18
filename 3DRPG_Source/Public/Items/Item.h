// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Item.generated.h"

class USphereComponent;
class UInventoryItem;
class AMyCharacter;
class UInteractWidgetComp;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class OPENSLASH_API AItem : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();
	virtual void Tick(float DeltaTime) override;

	virtual void Picking(AMyCharacter* NewOwner);

	virtual void BeginInteract(AMyCharacter* InteractingPlayer, AController* EventInstigator = nullptr) override;
	virtual void PopUpInteractWindow(bool IsPopUp) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;
	UFUNCTION(BlueprintPure)
	float TransformedSin();
	UFUNCTION(BlueprintPure)
	float TransformedCos();

	template<typename T>
	T Avg(T First, T Second);

	template<typename T>
	T* GenerateInvItem();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereCollision;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(VisibleAnywhere)
	int32 Amount;

	UPROPERTY(EditAnywhere)
	UInteractWidgetComp* InteractWidgetComp;
	
	UPROPERTY(EditAnywhere)
	FString ItemName;	
	
	UPROPERTY(EditAnywhere)
	int32 ItemID = 0;

public:
	inline void SetItemID(int32 NewID) { ItemID = NewID; }
	inline int32 GetItemID() { return ItemID; }
	inline UInteractWidgetComp* GetInteractWidgetComp() const { return InteractWidgetComp; }
};

template<typename T>
inline T AItem::Avg(T First, T Second)
{
	return (First + Second) / 2;
}

template <typename T>
T* AItem::GenerateInvItem()
{
	FString invStr;
	FString jsonStr;
	FFileHelper::LoadFileToString(jsonStr, *(FPaths::ProjectContentDir() + "/Data/ItemData.Json"));

	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(jsonStr);
	TSharedPtr<FJsonObject> jsonObj = MakeShareable(new FJsonObject());
	if (FJsonSerializer::Deserialize(jsonReader, jsonObj) && jsonObj.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>> jsonWeapons = jsonObj->GetArrayField(TEXT("WeaponList"));

		int32 itemID = this->GetItemID();
		itemID -= 101;

		if (!jsonWeapons.IsValidIndex(itemID))
		{
			UE_LOG(LogTemp, Error, TEXT("AItem::GenerateInvItem Error -> ItemID is Invalid"));
			return nullptr;
		}
		TSharedPtr<FJsonObject> jsonWeapon = jsonWeapons[itemID]->AsObject();
		invStr = jsonWeapon->GetStringField(TEXT("inventoryItemPath"));
	}

	FSoftClassPath invPath(*invStr);
	UClass* invClass = invPath.TryLoadClass<T>();
	if (invClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AItem::GenerateInvItem Error -> class path incorrect"));
		return nullptr;
	}
	T* invWeapon = NewObject<T>(this, invClass);
	if (invWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AItem::GenerateInvItem Error -> invWeapon doesn't spawn properly"));
		return nullptr;
	}
	return invWeapon;
}