// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "ActorComponents/AttributeComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Healing(float HealingAmount)
{
	Attributes->Healing(HealingAmount);
}

void ABaseCharacter::GetHit(float DamageAmount, const FVector& ImpactPoint, AActor* DamageCauser, AController* EventInstigator)
{
	if (Attributes)
	{
		Attributes->GetDamage(DamageAmount);
	}

	AfterGetHit(ImpactPoint);
	return;
}

void ABaseCharacter::AfterGetHit(const FVector& ImpactPoint)
{
	if (Attributes && !Attributes->IsAlive())
	{
		Die();
	}

	DisplayHitParticle(ImpactPoint);
	PlayHitSound(ImpactPoint);

	return;
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::Attack()
{
}

//---------------------------------------------Montage 를 어떻게 재생할 것인지에 대한 함수
// Montage 의 SectionName 파트의 길이를 리턴합니다.
float ABaseCharacter::GetMontageSectionLength(UAnimMontage* Montage, const FName SectionName)
{
	int32 sectionIndex = Montage->GetSectionIndex(SectionName);
	float sectionLength = Montage->GetSectionLength(sectionIndex);
	return sectionLength;
}

// Montage 에 얼마나 많은 section 이 있는지 리턴합니다.
int32 ABaseCharacter::GetNumOfMontageSections(const UAnimMontage* montage)
{
	if (montage == nullptr)
		return 0;
	return montage->CompositeSections.Num();
}

// Montage 의 SectionName 파트를 재생합니다.
void ABaseCharacter::PlayingMontage(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance != nullptr && Montage != nullptr)
	{
		if (Montage->IsValidSectionName(SectionName))
		{
			AnimInstance->Montage_Play(Montage);
			AnimInstance->Montage_JumpToSection(SectionName, Montage);
		}
	}
}

// Montage 의 SectionNumber 파트를 재생합니다.
void ABaseCharacter::PlayingMontage(UAnimMontage* Montage, const int32 SectionNumber)
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance != nullptr && Montage != nullptr)
	{
		FName sectionName = Montage->GetSectionName(SectionNumber);
		if (Montage->IsValidSectionName(sectionName))
		{
			animInstance->Montage_Play(Montage);
			animInstance->Montage_JumpToSection(sectionName, Montage);
		}
	}
}

// 입력받은 Montage 의 섹션 중 하나를 랜덤하게 골라 재생합니다.
// 랜덤하게 고른 섹션의 번호를 리턴합니다.
int32 ABaseCharacter::PlayingMontageRandSection(UAnimMontage* Montage)
{
	int32 sectionNum = GetNumOfMontageSections(Montage);
	int32 selection = FMath::RandRange(0, sectionNum - 1);
	PlayingMontage(Montage, selection);

	return selection;
}
//---------------------------------------------------------여기까지가 Montage를 어떻게 재생할 것인지에 대한 함수


//----------------------------------------------------------------------------구체적인 Montage를 재생하는 함수
void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	PlayingMontage(HitReactMontage, SectionName);
}

void ABaseCharacter::PlayDeathMontage(bool IsRagdoll)
{
	int32 selectedSection = PlayingMontageRandSection(DeathMontage);
	if (IsRagdoll)
	{
		float sectionLength = DeathMontage->GetSectionLength(selectedSection);
		GetWorldTimerManager().SetTimer(RagdollTimer, this, &ABaseCharacter::BeingRagdoll, sectionLength - BeforeRagdollTime);
	}
}

void ABaseCharacter::PlayAttackMontage()
{
}
//----------------------------------------------------------------------------구체적인 Montage를 재생하는 함수

void ABaseCharacter::BeingRagdoll()
{
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetSimulatePhysics(true);
	GetWorldTimerManager().ClearTimer(RagdollTimer);
}

void ABaseCharacter::HitReaction(const FVector& ImpactPoint)
{
	const FVector forward = GetActorForwardVector();

	const FVector impactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector toHit = (impactLowered - GetActorLocation()).GetSafeNormal();

	const double cosTheta = FVector::DotProduct(forward, toHit);
	double theta = FMath::Acos(cosTheta);

	theta = FMath::RadiansToDegrees(theta);
	const FVector crossProduct = FVector::CrossProduct(forward, toHit);
	if (crossProduct.Z < 0)
	{
		theta *= -1.f;
	}

	FName Section("MoveFront");

	if (theta >= -45.f && theta < 45.f)
	{
		Section = FName("MoveBack");
	}
	else if (theta >= -135.f && theta < -45.f)
	{
		Section = FName("MoveRight");
	}
	else if (theta >= 45.f && theta < 135.f)
	{
		Section = FName("MoveLeft");
	}

	PlayHitReactMontage(Section);
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage(true);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (!IsEquipWeapon())
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: (BaseCharacter::SetWeaponCollisionEnabled) There is no equipped weapon."));
		return;
	}
	for (AWeapon* equippedWeapon : EquippedWeapons)
	{
		equippedWeapon->SetWeaponCollisionBoxState(CollisionEnabled);
	}
}

void ABaseCharacter::SetWeaponCollisionByNum(int32 SocketNumber, ECollisionEnabled::Type CollisionEnabled)
{
	if (!IsEquipWeapon(SocketNumber))
	{
		UE_LOG(LogTemp, Warning, TEXT("Error: (BaseCharacter::SetWeaponCollisionByNum) There is no weapon equipped for the given socket number."));
		return;
	}
	EquippedWeapons[SocketNumber]->SetWeaponCollisionBoxState(CollisionEnabled);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UE_LOG(LogTemp, Warning, TEXT("Play Hit sound"));
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}
void ABaseCharacter::DisplayHitParticle(const FVector& ImpactPoint)
{
	if (HitParticles && GetWorld())
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn Particle"));
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
		);
	}
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABaseCharacter::IsEquipWeapon()
{
	if (EquippedWeapons.IsEmpty())
	{
		return false;
	}
	if (!EquippedWeapons.IsValidIndex(0))
	{
		return false;
	}

	return true;
}

bool ABaseCharacter::IsEquipWeapon(int32 SocketNumber)
{
	if (EquippedWeapons.IsEmpty())
	{
		return false;
	}
	if (!EquippedWeapons.IsValidIndex(SocketNumber))
	{
		return false;
	}

	return true;
}