// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/InGameHUD.h"
#include "Widgets/MyCharacterOverlay.h"

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* controller = world->GetFirstPlayerController();
		if (controller && MyCharacterOverlayClass)
		{
			MyCharacterOverlay = CreateWidget<UMyCharacterOverlay>(controller, MyCharacterOverlayClass);
			MyCharacterOverlay->AddToViewport();
		}
	}
}