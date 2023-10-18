// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

void UAttributeComponent::Healing(float HealAmount) 
{
	HP += HealAmount;

	if (HP >= MaxHP)
	{
		HP = MaxHP;
	}
}

// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}
// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UAttributeComponent::GetDamage(float Damage)
{
	HP = FMath::Clamp(HP - Damage, 0.f, MaxHP);
}

void UAttributeComponent::GainEXP(float EXPAmount)
{
	EXP += EXPAmount;
	if (EXP >= MaxEXP)
	{
		LevelUp();
	}
}

void UAttributeComponent::LevelUp()
{
	Level++;
	StatPoint++;
	ResetEXP();
	ResetHP();
}

void UAttributeComponent::ResetEXP()
{
	EXP -= MaxEXP;
	MaxEXP += 50;
}

void UAttributeComponent::ResetHP()
{
	MaxHP += 50;
	HP = MaxHP;
}

float UAttributeComponent::GetHPPercent()
{
	return HP / MaxHP;
}

float UAttributeComponent::GetEXPPercent()
{
	return EXP / MaxEXP;
}

bool UAttributeComponent::IsAlive()
{
	return HP > 0.f;
}