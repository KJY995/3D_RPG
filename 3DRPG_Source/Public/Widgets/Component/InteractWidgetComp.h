// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "InteractWidgetComp.generated.h"

/**
 * 
 */
UCLASS()
class OPENSLASH_API UInteractWidgetComp : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void PopUp();
private:
	UPROPERTY()
	class UInteractionWidget* InteractionWidget;
	
};
