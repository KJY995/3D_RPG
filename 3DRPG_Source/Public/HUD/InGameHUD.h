// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"


class UMyCharacterOverlay;
/**
 * 
 */
UCLASS()
class OPENSLASH_API AInGameHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditDefaultsOnly, Category = OpenSlash)
	TSubclassOf<UMyCharacterOverlay> MyCharacterOverlayClass;

	UPROPERTY()
	UMyCharacterOverlay* MyCharacterOverlay;

public:
	FORCEINLINE UMyCharacterOverlay* GetOverlay() const { return MyCharacterOverlay; }

};