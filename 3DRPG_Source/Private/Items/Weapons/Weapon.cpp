// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Character/MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "Inventory/InventoryItemWeapon.h"

AWeapon::AWeapon()
{
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ItemState != EItemState::EIS_Equipped)
	{
		Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	}
}
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ItemState != EItemState::EIS_Equipped)
	{
		Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	}
}

void AWeapon::SetStateToEquipped(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	PlayEquipSound();
	SetSphereOverlapEnable(false);
	ResetIgnoreActorsArray();
}

void AWeapon::SetSphereOverlapEnable(bool Enable)
{
	if (!SphereCollision)
	{
		return;
	}
	if (Enable)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent * Parent, const FName & SocketName)
{
	FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(Parent, transformRules, SocketName);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 첫 번째, ignore Attack을 넘겨주기 위하여 깊은 복사를 해주자.
	TArray<AActor*> actorsToIgnore = IgnoreActors;
	FHitResult boxHitResult;

	//두 번째, 피격 판정 결과를 AttackResult 를 통해 뽑아낸다.
	//판정 결과 피격이 존재하지 않았다면 그대로 return 한다.
	if (AttackResult(actorsToIgnore, boxHitResult) == false) return;

	// 세 번째, 피격자를 IgnoreActors 에 포함시킨다.
	// 만약 피격자와 공격자가 같은 태그를 달고 있다면 그대로 return 한다.
	AActor* victim = boxHitResult.GetActor();
	IgnoreActors.AddUnique(victim);
	if (ActorIsSameType(victim))
	{
		return;
	}

	// 네 번째, 맞은 적의 GetHit 을 호출한다.
	AController* eventInstigator = this->GetInstigatorController();
	ExecuteGetHit(victim, boxHitResult.ImpactPoint, this, eventInstigator);

	//디버깅
	UE_LOG(LogTemp, Warning, TEXT("%s"), *victim->GetName());

	// 1) 이 후, 무기를 휘두르는게 끝난다면 
	// ("Attack Anim Montage" 에서 "DisableCollision" 라는 Notify에 도달하게 되면)
	// 
	// 2) 무기를 휘두른 캐릭터의 Animation Blueprint 쪽에서
	// Base 캐릭터의 SetWeaponCollisionEnabled 를 호출한다.
	// 
	// 3) 그 함수는 Weapon의 SetWeaponCollisionBoxState 를 호출하여 NoCollision 상태로 만들고 동시에
	// IgnoreActors 를 리셋한다.
	// 
	// 4) 다시 무기를 휘두른다면, 캐릭터 쪽에서 AnimMontage를 재생하며 특정 Notify 가 트리거되고,
	// 그러면 Animation Blueprint 에서 SetWeaponCollisionEnabled 를 호출하며 다시금 오버랩 이벤트가 시작된다.
}

// 오버랩 이벤트에서 얻을 수 있는 FHitResult& SweepResult 에서는 임팩트 포인트를 얻을 수 없다.
// 버그인지 미구현인지 모르겠으나, 덕분에 BoxTraceSingle 같은 걸 통해 일을 두 번 해야만 한다.
// BoxHitResult를 통해 피격 판정 결과를 출력한다.
bool AWeapon::AttackResult(TArray<AActor*> ActorsToIgnore, FHitResult &BoxHitResult)
{
	const FVector start = BoxTraceStart->GetComponentLocation();
	const FVector end = BoxTraceEnd->GetComponentLocation();

	FHitResult boxHit;
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		start,
		end,
		BoxTraceSize,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		boxHit,
		true
	);

	if (boxHit.GetActor())
	{
		BoxHitResult = boxHit;
		return true;
	}
	else
	{
		return false;
	}
}

void AWeapon::ExecuteGetHit(AActor* Victim, FVector ImpactPoint, AActor* DamageCauser, AController* EventInstigator)
{
	IHitInterface* hitInterface = Cast<IHitInterface>(Victim);
	if (hitInterface != nullptr)
	{
		hitInterface->GetHit(Damage, ImpactPoint, DamageCauser, EventInstigator);
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	AActor* owner =	this->GetOwner();
	for (FName tag : owner->Tags)
	{
		for (FName otherActorTag : OtherActor->Tags)
		{
			if (tag == otherActorTag)
			{
				IgnoreActors.AddUnique(OtherActor);
				return true;
			}
		}
	}
	return false;
}

void AWeapon::SetWeaponCollisionBoxState(ECollisionEnabled::Type CollisionEnabled)
{
	if (this->WeaponCollisionBox)
	{
		WeaponCollisionBox->SetCollisionEnabled(CollisionEnabled);
		ResetIgnoreActorsArray();
	}
}

void AWeapon::ResetIgnoreActorsArray()
{
	this->IgnoreActors.Empty();
	IgnoreActors.AddUnique(this);
	if (this->GetOwner())
	{
		IgnoreActors.AddUnique(this->GetOwner());
	}
}

void AWeapon::Picking(AMyCharacter* NewOwner)
{
	UInventoryItemWeapon* invWeapon = GenerateInvWeapon();

	if (invWeapon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AWeapon::Picking Error -> invWeapon Generation failed (return nullptr)"));
		return;
	}
	NewOwner->AddItemToInventory(invWeapon, 1);
	Destroy();
}

UInventoryItemWeapon* AWeapon::GenerateInvWeapon()
{
	FString invStr;
	FString jsonStr;
	FFileHelper::LoadFileToString(jsonStr, *(FPaths::ProjectContentDir() + "/Data/ItemData.Json"));

	TSharedRef<TJsonReader<TCHAR>> jsonReader = TJsonReaderFactory<TCHAR>::Create(jsonStr);
	TSharedPtr<FJsonObject> jsonObj = MakeShareable(new FJsonObject());
	if (FJsonSerializer::Deserialize(jsonReader, jsonObj) && jsonObj.IsValid())
	{
		const TArray<TSharedPtr<FJsonValue>> jsonWeapons = jsonObj->GetArrayField(TEXT("WeaponList"));

		int32 itemID = this->GetItemID();
		itemID -= 101;

		if (!jsonWeapons.IsValidIndex(itemID))
		{
			UE_LOG(LogTemp, Error, TEXT("AWeapon::GenerateInvItem Error -> WeaponID is Invalid"));
			return nullptr;
		}
		TSharedPtr<FJsonObject> jsonWeapon = jsonWeapons[itemID]->AsObject();
		invStr = jsonWeapon->GetStringField(TEXT("inventoryItemPath"));
	}

	FSoftClassPath invPath(*invStr);
	UClass* invClass = invPath.TryLoadClass<UInventoryItemWeapon>();
	if (invClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::GenerateInvItem Error -> class path incorrect"));
		return nullptr;
	}
	UInventoryItemWeapon* invWeapon = NewObject<UInventoryItemWeapon>(this, invClass);
	if (invWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AWeapon::GenerateInvItem Error -> invWeapon doesn't spawn properly"));
		return nullptr;
	}
	return invWeapon;
}