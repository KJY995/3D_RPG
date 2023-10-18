// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Component/HPBarComponent.h"
#include "Widgets/HPBarWidget.h"
#include "Components/ProgressBar.h"

void UHPBarComponent::SetHPPercent(float Percent)
{
	if (HPBarWidget == nullptr)
	{
		HPBarWidget = Cast<UHPBarWidget>(GetUserWidgetObject());
	}

	if (HPBarWidget && HPBarWidget->HPBar)
	{
		HPBarWidget->HPBar->SetPercent(Percent);
	}
}