// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/MyCharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMyCharacterOverlay::SetHPBarPercent(float Percent)
{
	if (HPBar)
	{
		HPBar->SetPercent(Percent);
	}
}

void UMyCharacterOverlay::SetHPText(float HP, float MaxHP)
{
	if (HPText)
	{
		const FString string = FString::Printf(TEXT("%d / %d"), (int)HP, (int)MaxHP);
		const FText textForHP = FText::FromString(string);
		HPText->SetText(textForHP);
	}
}

void UMyCharacterOverlay::SetEXPBarPercent(float Percent)
{
	if (EXPBar)
	{
		EXPBar->SetPercent(Percent);
	}
}

void UMyCharacterOverlay::SetLevelText(int Level)
{

}
