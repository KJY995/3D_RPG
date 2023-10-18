// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AWeapon;
class UMyCharacterOverlay;
class UInventoryComponent;
class UInventoryItem;
class UInventoryItemWeapon;
class IInteractInterface;

UCLASS()
class OPENSLASH_API AMyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	virtual void Healing(float HealingAmount) override;

	UFUNCTION(BlueprintCallable, Category = "Items")
	void UseItem(UInventoryItem* Item);

	void EquipItem(UInventoryItem* Item);

	void AddItemToInventory(UInventoryItem* Item, int32 Amount);

	virtual void AddEXP(float EXPAmount);

	void ToggleWidget();

	void SetOverlappedObj(AActor* NewOverlappedObj);

protected:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	virtual void BeginPlay() override;
	void EKeyPressed();
	void PlayLevelUpEffect();

	virtual void Attack() override;
	virtual void PlayAttackMontage() override;
	virtual void AttackEnd() override;

	virtual bool CanAttack() override;

	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	virtual void Die() override;

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	virtual void AfterGetHit(const FVector& ImpactPoint) override;
private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleInstanceOnly)
	AActor* OverlappedObj;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	void InitializeOverlay();

	UPROPERTY()
	UMyCharacterOverlay* Overlay;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent;

	AWeapon* GenerateWeapon(UInventoryItemWeapon* InvWeapon);

	void SetOverlayBar();

public:
	inline ECharacterState GetCharacterState() const { return CharacterState; }
	inline EActionState GetActionState() const { return ActionState; }

	UFUNCTION(BlueprintCallable)
	inline UInventoryComponent* GetInvComp() const { return InventoryComponent; }
};
