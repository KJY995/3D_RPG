// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "HUD/InGameHUD.h"
#include "Widgets/MyCharacterOverlay.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/InventoryItemWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/InventoryWidget.h"
#include "JsonObjectConverter.h"
#include "Templates/SharedPointer.h"
#include "Misc/FileHelper.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory");
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("AttackableTarget"));
	InitializeOverlay();
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMyCharacter::LookUp);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &AMyCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Interact"), IE_Pressed, this, &AMyCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AMyCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Tab"), IE_Pressed, this, &AMyCharacter::ToggleWidget);
}

void AMyCharacter::Jump()
{
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		Super::Jump();
	}
}


// 이 함수는 인벤토리 상에서 어떤 아이템을 클릭하면 블루프린트를 통하여 호출된다.
// 좀 더 구체적으로는, WBP_InventoryItem 내에 UseButton이라는 요소가 있는데,
// UseButton을 클릭하면 MyCharacter를 가져와서 이 함수를 호출한다.
void AMyCharacter::UseItem(UInventoryItem* Item)
{
	if (Item == nullptr)
	{
		return;
	}
	Item->UseThisItem(this);
	InventoryComponent->Refresh();
}

void AMyCharacter::EquipItem(UInventoryItem* Item)
{
	if (Item->GetItemType() != EItemType::EIT_Equippable) return;

	if (IsEquipWeapon())
	{
		for (AWeapon* equippedWeapon : EquippedWeapons)
		{
			equippedWeapon->Destroy();
		}
	}
	UInventoryItemWeapon* invWeapon = Cast<UInventoryItemWeapon>(Item);

	UWorld* world = GetWorld();
	if (world != nullptr && invWeapon !=nullptr)
	{
		AWeapon* equippingWeapon = GenerateWeapon(invWeapon);
		equippingWeapon->SetStateToEquipped(GetMesh(), FName("RightHandSocket"), this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		EquippedWeapons.Emplace(equippingWeapon);
		if (OverlappedObj == equippingWeapon)
		{
			SetOverlappedObj(nullptr);
		}
	}
}

AWeapon* AMyCharacter::GenerateWeapon(UInventoryItemWeapon* InvWeapon)
{
	FString weaponStr;
	FString jsonStr;
	FFileHelper::LoadFileToString(jsonStr, *(FPaths::ProjectContentDir() + "/Data/ItemData.Json"));

	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(jsonStr);
	TSharedPtr<FJsonObject> jsonObj = MakeShareable(new FJsonObject());
	if (FJsonSerializer::Deserialize(jsonReader, jsonObj) && jsonObj.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>> jsonWeapons = jsonObj->GetArrayField(TEXT("WeaponList"));

		int32 itemID = InvWeapon->GetItemID();
		itemID -= 101;

		if (!jsonWeapons.IsValidIndex(itemID))
		{
			UE_LOG(LogTemp, Error, TEXT("AMyCharacter::EquipItem Error -> WeaponID is Invalid"));
			return nullptr;
		}
		TSharedPtr<FJsonObject> jsonWeapon = jsonWeapons[itemID]->AsObject();
		weaponStr = jsonWeapon->GetStringField(TEXT("actorPath"));
	}

	FSoftClassPath weaponPath(*weaponStr);
	UClass* weaponClass = weaponPath.TryLoadClass<AWeapon>();
	FActorSpawnParameters spawnParam;
	if (weaponClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMyCharacter::EquipItem Error -> class path incorrect"));
		return nullptr;
	}
	AWeapon* equippingWeapon = GetWorld()->SpawnActor<AWeapon>(weaponClass, GetActorLocation(), GetActorRotation(), spawnParam);
	if (equippingWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AMyCharacter::EquipItem Error -> Weapon doesn't spawn properly"));
		return nullptr;
	}
	return equippingWeapon;
}

void AMyCharacter::AddItemToInventory(UInventoryItem* Item, int32 Amount)
{
	InventoryComponent->AddItem(Item, Amount);
}

void AMyCharacter::AfterGetHit(const FVector& ImpactPoint)
{
	SetOverlayBar();

	if (Attributes && !Attributes->IsAlive())
	{
		Die();
	}

	PlayHitSound(ImpactPoint);
	DisplayHitParticle(ImpactPoint);
}

void AMyCharacter::Healing(float HealingAmount)
{
	Super::Healing(HealingAmount);
	SetOverlayBar();
}

void AMyCharacter::ToggleWidget()
{	
	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	if (!InventoryComponent->IsWidgetInViewport())
	{
		InventoryComponent->PopInventoryWidget();
		playerController->SetShowMouseCursor(true);
		FInputModeUIOnly UIOnly;
		playerController->SetInputMode(UIOnly);
	}
	else
	{
		InventoryComponent->OffInventoryWidget();
		playerController->SetShowMouseCursor(false);
		FInputModeGameOnly GameOnly;
		playerController->SetInputMode(GameOnly);
	}
}

void AMyCharacter::SetOverlappedObj(AActor* NewOverlappedObj)
{
	IInteractInterface* interactiveObj;
	if (OverlappedObj != nullptr)
	{
		interactiveObj = Cast<IInteractInterface>(OverlappedObj);
		interactiveObj->PopUpInteractWindow(false);
	}
	OverlappedObj = NewOverlappedObj;
	if (OverlappedObj != nullptr)
	{
		interactiveObj = Cast<IInteractInterface>(OverlappedObj);
		interactiveObj->PopUpInteractWindow(true);
	}
}

void AMyCharacter::EKeyPressed()
{
	if (OverlappedObj != nullptr)
	{
		IInteractInterface* interactiveObj = Cast<IInteractInterface>(OverlappedObj);
		if (interactiveObj != nullptr)
		{
			interactiveObj->BeginInteract(this);
		}
	}
}

void AMyCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		const FRotator controlRotation = GetControlRotation();
		const FRotator yawRotation(0.f, controlRotation.Yaw, 0.f);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(direction, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		const FRotator controlRotation = GetControlRotation();
		const FRotator yawRotation(0.f, controlRotation.Yaw, 0.f);

		const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(direction, Value);
	}
}

void AMyCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMyCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMyCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

//----------------------------------------------------------------------------
bool AMyCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMyCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMyCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		IsEquipWeapon();
}

void AMyCharacter::Disarm()
{
	if (IsEquipWeapon())
	{
		for (AWeapon* equippedWeapon : EquippedWeapons)
		{
			equippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
		}
	}
}

void AMyCharacter::Arm()
{
	if (IsEquipWeapon())
	{
		for (AWeapon* equippedWeapon : EquippedWeapons)
		{
			equippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
		}
	}
}
//-------------------------------------------------------------------------------------------

void AMyCharacter::Die()
{
	Super::Die();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void AMyCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMyCharacter::PlayAttackMontage()
{
	Super::PlayAttackMontage();
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && AttackMontage)
	{
		PlayingMontageRandSection(AttackMontage);
	}
}

void AMyCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && EquipMontage)
	{
		animInstance->Montage_Play(EquipMontage);
		animInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}

}
void AMyCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}


void AMyCharacter::AddEXP(float EXPAmount)
{
	int currentLevel = Attributes->GetLevel();
	Attributes->GainEXP(EXPAmount);

	if (currentLevel != Attributes->GetLevel())
	{
		PlayLevelUpEffect();
	}
	SetOverlayBar();
}

void AMyCharacter::PlayLevelUpEffect()
{

}

void AMyCharacter::InitializeOverlay()
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (playerController)
	{
		AInGameHUD* myCharacterHUD = Cast<AInGameHUD>(playerController->GetHUD());
		if (myCharacterHUD)
		{
			Overlay = myCharacterHUD->GetOverlay();
			if (Overlay && Attributes)
			{
				SetOverlayBar();
			}
		}
	}
}

void AMyCharacter::SetOverlayBar()
{
	if (Overlay && Attributes)
	{
		Overlay->SetHPBarPercent(Attributes->GetHPPercent());
		Overlay->SetHPText(Attributes->GetCurrentHP(), Attributes->GetMaxHP());

		float expPercent = Attributes->GetEXPPercent();
		Overlay->SetEXPBarPercent(expPercent);
	}
}