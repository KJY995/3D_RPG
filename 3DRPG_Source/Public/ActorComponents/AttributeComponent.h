// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OPENSLASH_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//void ReceiveDamage(float Damage);
	void GetDamage(float Damage);

	void Healing(float HealAmount);
	void GainEXP(float EXPAmount);
	void LevelUp();

	float GetHPPercent();
	float GetEXPPercent();
	bool IsAlive();


	inline float GetCurrentHP() { return HP; }
	inline float GetMaxHP() { return MaxHP; }
	inline float GetCurrentEXP() { return EXP; }
	inline float GetMaxEXP() { return MaxEXP; }

	inline int GetLevel() { return Level; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void ResetEXP();
	void ResetHP();

private:
	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	int Level;

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float HP;

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float MaxHP;

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float EXP;

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	float MaxEXP = 99999.f;

	UPROPERTY(EditAnywhere, Category = "ActorAttributes")
	int StatPoint = 1;
};