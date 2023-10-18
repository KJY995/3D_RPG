// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InteractionWidget.h"
#include "Components/TextBlock.h"

void UInteractionWidget::SetName(FString* NewName)
{
	InteractName->SetText(FText::FromString(*NewName));
}