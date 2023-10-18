// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class OPENSLASH_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionByNum(int32 SocketNumber, ECollisionEnabled::Type CollisionEnabled);

	virtual void Healing(float HealingAmount);

	virtual bool IsEquipWeapon();
	virtual bool IsEquipWeapon(int32 SocketNumber);

protected:

	virtual void PlayHitSound(const FVector& ImpactPoint);
	virtual void DisplayHitParticle(const FVector& ImpactPoint);

	virtual void GetHit(float DamageAmount, const FVector& ImpactPoint, AActor* DamageCauser, AController* EventInstigator) override;
	virtual void AfterGetHit(const FVector& ImpactPoint);

	virtual void BeginPlay() override;
	virtual void Attack();
	virtual void Die();

	int32 GetNumOfMontageSections(const UAnimMontage* montage);
	float GetMontageSectionLength(UAnimMontage* Montage, const FName SectionName);

	virtual void PlayingMontage(UAnimMontage* Montage, const FName& SectionName);
	virtual void PlayingMontage(UAnimMontage* Montage, const int32 SectionNumber);
	int32 PlayingMontageRandSection(UAnimMontage* Montage);

	
	virtual void PlayAttackMontage();
	void PlayHitReactMontage(const FName& SectionName);
	void HitReaction(const FVector& ImpactPoint);
	virtual void PlayDeathMontage(bool IsRagdoll = true);

	void BeingRagdoll();

	void DisableMeshCollision();

	virtual bool CanAttack();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();


	FTimerHandle RagdollTimer;

	UPROPERTY(EditAnywhere)
	float BeforeRagdollTime = 0.05f;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TArray<AWeapon*> EquippedWeapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* HitParticles;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* DeathMontage;
};
