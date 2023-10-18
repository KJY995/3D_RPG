// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyCharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class OPENSLASH_API UMyCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetHPBarPercent(float Percent);
	void SetEXPBarPercent(float Percent);
	void SetHPText(float HP, float MaxHP);
	void SetLevelText(int Level);
	//void SetGold(int32 Gold);
	//void SetSouls(int32 Souls);

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HPBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* EXPBar;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HPText;
};