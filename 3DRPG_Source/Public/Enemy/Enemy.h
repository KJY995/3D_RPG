// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Character/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHPBarComponent;
class UPawnSensingComponent;

UCLASS()
class OPENSLASH_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	
	virtual void GetHit(float DamangeAmount, const FVector& ImpactPoint, AActor* DamageCauser, AController* EventInstigator) override;
	virtual void AfterGetHit(const FVector& ImpactPoint) override;

	void CheckPatrolTarget();
	void CheckCombatTarget();

	virtual void Destroyed() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Die() override;
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);

	AActor* ChoosePatrolTarget();

	virtual void Attack() override;
	virtual void PlayAttackMontage() override;

	void RotateToCombatTarget();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	virtual bool DisplayHPBar(bool Display);
	virtual void ChangeStateAndAction(EEnemyState NextEnemyState);
	virtual void OnAttackMode();
	virtual void OffAttackMode();
	virtual void AttackEnd() override;

	virtual void ChaseAfterGetHit(float WaitAfterHit);
	bool ActorIsSameType(AActor* OtherActor);
	
	void EquipAll();

private:
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class AWeapon>> WeaponClasses;

	UPROPERTY(EditAnywhere)
	FString WeaponSocketName = TEXT("WeaponSocket");

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1000.f;

	UPROPERTY(EditAnywhere)
	float RewardEXP = 100.f;

	UPROPERTY(EditAnywhere)
	float ReachToTargetRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	UHPBarComponent* HPBarComponent;

	UPROPERTY()
	class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;
	
	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY(EditAnywhere)
	double NormalSpeed = 150.f;
	
	UPROPERTY(EditAnywhere)
	double ChaseSpeed = 350.f;
	
	UPROPERTY(EditAnywhere)
	double FastestMovingSpeed = 550.f;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	
	UPROPERTY(EditAnywhere)
	float WaitAfterHitTime = 1.f;
};