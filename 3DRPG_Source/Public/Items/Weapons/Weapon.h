// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
class USoundBase;
class UInventoryItemWeapon;

UCLASS()
class OPENSLASH_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();
	void SetStateToEquipped(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void SetSphereOverlapEnable(bool Enable);
	void PlayEquipSound();
	void AttachMeshToSocket(USceneComponent* Parent, const FName& SocketName);
	virtual void SetWeaponCollisionBoxState(ECollisionEnabled::Type CollisionEnabled);

	virtual void Picking(AMyCharacter* NewOwner) override;

	void ResetIgnoreActorsArray();

	UPROPERTY()
	TArray<AActor*> IgnoreActors;

protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	virtual void ExecuteGetHit(AActor* Victim, FVector ImpactPoint, AActor* DamageCauser, AController* EventInstigator);

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	bool AttackResult(TArray<AActor*> ActorsToIgnore, FHitResult& BoxHitResult);
	
	bool ActorIsSameType(AActor* OtherActor);
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponCollisionBox;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceSize = FVector(0.f);

	UInventoryItemWeapon* GenerateInvWeapon();
public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const { return WeaponCollisionBox; }
};