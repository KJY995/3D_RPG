// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HPBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class OPENSLASH_API UHPBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHPPercent(float Percent);
private:
	UPROPERTY()
	class UHPBarWidget* HPBarWidget;
};
