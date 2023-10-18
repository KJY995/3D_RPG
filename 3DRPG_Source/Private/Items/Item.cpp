// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "Components/SphereComponent.h"
#include "Character/MyCharacter.h"
#include "Inventory/InventoryItem.h"
#include "Widgets/Component/InteractWidgetComp.h"
#include "Widgets/InteractionWidget.h"
#include "Inventory/InventoryItemWeapon.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereCollision->SetupAttachment(GetRootComponent());

	InteractWidgetComp = CreateDefaultSubobject<UInteractWidgetComp>(TEXT("InteractWidgetComp"));
	InteractWidgetComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	UInteractionWidget* interactWidget = Cast<UInteractionWidget>(InteractWidgetComp->GetUserWidgetObject());
	if (interactWidget != nullptr)
	{
		interactWidget->SetName(&ItemName);
	}
}


float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}
float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMyCharacter* myCharacter = Cast<AMyCharacter>(OtherActor);
	if (myCharacter != nullptr)
	{
		myCharacter->SetOverlappedObj(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMyCharacter* myCharacter = Cast<AMyCharacter>(OtherActor);
	if (myCharacter != nullptr)
	{
		myCharacter->SetOverlappedObj(nullptr);
	}
}

void AItem::BeginInteract(AMyCharacter* InteractingPlayer, AController* EventInstigator)
{
	if (InteractingPlayer != nullptr)
	{
		Picking(InteractingPlayer);
	}
}

void AItem::PopUpInteractWindow(bool IsPopUp)
{
	InteractWidgetComp->SetVisibility(IsPopUp);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}


void AItem::Picking(AMyCharacter* NewOwner)
{
	UInventoryItem* invItem = GenerateInvItem<UInventoryItem>();
	if (invItem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AItem::Picking Error -> invItem Generation failed (return nullptr)"));
		return;
	}
	NewOwner->AddItemToInventory(invItem, 1);
	Destroy();
}