// Fill out your copyright notice in the Description page of Project Settings.
#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActorComponents/AttributeComponent.h"
#include "Widgets/Component/HPBarComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "OpenSlash/DebugMacros.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Character/MyCharacter.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/App.h"

#include "time.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HPBarComponent = CreateDefaultSubobject<UHPBarComponent>(TEXT("HPBar"));
	HPBarComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	Tags.Add(FName("Enemy"));
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	DisplayHPBar(false);

	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	EquipAll();
}

void AEnemy::EquipAll()
{
	UWorld* world = GetWorld();
	if (world == nullptr && WeaponClasses.Num() == 0)
	{
		return;
	}

	int32 loopCount = 0;
	for (TSubclassOf<AWeapon> WeaponClass : WeaponClasses)
	{
		WeaponSocketName += FString::FromInt(loopCount);
		FName socketFName = FName(*WeaponSocketName);
		WeaponSocketName.RemoveFromEnd(FString::FromInt(loopCount));

		AWeapon* defaultWeapon = world->SpawnActor<AWeapon>(WeaponClass);
		defaultWeapon->SetStateToEquipped(GetMesh(), socketFName, this, this);
		EquippedWeapons.Emplace(defaultWeapon);

		loopCount++;
	}
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState != EEnemyState::EES_Dead)
	{
		if (EnemyState > EEnemyState::EES_Patrolling)
		{
			CheckCombatTarget();
		}
		else
		{
			CheckPatrolTarget();
		}
	}
}

//-------------------------------------------------------------------- 적 행동 패턴

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double distanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return distanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr || EnemyState == EEnemyState::EES_Attacking) return;
	FAIMoveRequest moveRequest;
	moveRequest.SetGoalActor(Target);
	moveRequest.SetAcceptanceRadius(ReachToTargetRadius);
	EnemyController->MoveTo(moveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> validTargets;
	for (AActor* target : PatrolTargets)
	{
		if (target != PatrolTarget)
		{
			validTargets.AddUnique(target);
		}
	}

	const int32 numPatrolTargets = validTargets.Num();
	if (numPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, numPatrolTargets - 1);
		return validTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float waitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, waitTime);
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::CheckCombatTarget()
{
	if (EnemyState == EEnemyState::EES_Attacking) return;
	// 플레이어가 전투 범위 밖으로 나가면
	if (!InTargetRange(CombatTarget, CombatRadius))
{
		// 흥미를 잃고 패트롤 시작
		ChangeStateAndAction(EEnemyState::EES_Patrolling);
	}
	// 플레이어가 전투 범위 안에 있으나 공격의 사정거리 밖일 경우
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
{
		// 플레이어를 쫓아간다
		ChangeStateAndAction(EEnemyState::EES_Chasing);
	}
	// 플레이어가 사정거리 안에 있다면
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		// 공격한다
		ChangeStateAndAction(EEnemyState::EES_Attacking);
	}
}

void AEnemy::ChangeStateAndAction(EEnemyState NextEnemyState)
{
	if (EnemyState == EEnemyState::EES_Dead) return;
	switch (NextEnemyState)
	{
	case EEnemyState::EES_Patrolling:
		// 플레이어에 대한 흥미를 잃는다. 공격 타이머가 초기화된다.
		CombatTarget = nullptr;
		DisplayHPBar(false);
		// 느린 속도로 정찰을 시작한다
		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
		if (PatrolTarget != nullptr) 
		{ 
			MoveToTarget(PatrolTarget); 
		}
		break;
	case EEnemyState::EES_Chasing:
		// 빠른 속도로 플레이어를 쫓아간다
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		if (CombatTarget != nullptr)
		{ 
			MoveToTarget(CombatTarget); 
		}
		break;
	case EEnemyState::EES_Attacking:
		// 매 시간마다 공격하는 공격 타이머를 킨다
		EnemyState = EEnemyState::EES_Attacking;
		OnAttackMode();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("ERROR: ChangeState Doesn't Work!!!"));
		break;
	}
}

void AEnemy::OnAttackMode()
{
	if (!GetWorld() || !EnemyController) return;
	EnemyController->StopMovement();
	Attack();
}

void AEnemy::OffAttackMode()
{
	if (EnemyState == EEnemyState::EES_Dead) return;
	ChangeStateAndAction(EEnemyState::EES_Chasing);
}

void AEnemy::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}

	if (CombatTarget == nullptr)
	{
		ChangeStateAndAction(EEnemyState::EES_Patrolling);
		return;
	}

	PlayAttackMontage();
}

void AEnemy::AttackEnd()
{
	OffAttackMode();
}

void AEnemy::PlayAttackMontage()
{
	RotateToCombatTarget();

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && AttackMontage)
	{
		PlayingMontageRandSection(AttackMontage);
	}
}

void AEnemy::RotateToCombatTarget()
{
	FVector3d enemyForward = GetActorForwardVector();

	FVector3d newForward = -this->GetActorLocation() + CombatTarget->GetActorLocation();

	newForward.Z = 0;
	newForward = newForward.GetSafeNormal();

	double dotProductResult = FVector3d::DotProduct(enemyForward, newForward);
	double theta = FMath::Acos(dotProductResult);
	theta = FMath::RadiansToDegrees(theta);

	FVector crossProduct = FVector::CrossProduct(enemyForward, newForward);

	if (crossProduct.Z < 0)
	{
		theta *= -1.f;
	}

	FRotator actorRotate(GetActorRotation().Pitch, GetActorRotation().Yaw + theta, GetActorRotation().Roll);
	SetActorRotation(actorRotate);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState != EEnemyState::EES_Patrolling) return;

	if (SeenPawn->ActorHasTag(FName("AttackableTarget")))
	{
		CombatTarget = SeenPawn;
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		ChangeStateAndAction(EEnemyState::EES_Chasing);
	}
}

bool AEnemy::ActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

//----------------------------------------------------------------- 피격 및 사망 처리
void AEnemy::AfterGetHit(const FVector& ImpactPoint)
{
	if (EnemyState == EEnemyState::EES_Dead) return;

	if (HPBarComponent != nullptr)
	{
		HPBarComponent->SetHPPercent(Attributes->GetHPPercent());
		DisplayHPBar(true);
	}

	if (Attributes != nullptr && Attributes->IsAlive())
	{
		if (EnemyState != EEnemyState::EES_Attacking)
		{
			HitReaction(ImpactPoint);
		}
	}
	else
	{
		Die();

		if (CombatTarget->IsA(AMyCharacter::StaticClass()))
		{
			AMyCharacter* slayedPlayer = Cast<AMyCharacter>(CombatTarget);
			slayedPlayer->AddEXP(RewardEXP);
		}
	}

	DisplayHitParticle(ImpactPoint);
	PlayHitSound(ImpactPoint);

	ChaseAfterGetHit(WaitAfterHitTime);
}

void AEnemy::ChaseAfterGetHit(float WaitAfterHit)
{
	if (EnemyState != EEnemyState::EES_Dead)
	{
		FTimerHandle waitAfterHitTimer;
		GetWorld()->GetTimerManager().SetTimer(waitAfterHitTimer, FTimerDelegate::CreateLambda([&]()
		{
			if (EnemyState != EEnemyState::EES_Attacking)
			{
				ChangeStateAndAction(EEnemyState::EES_Chasing);

			}
		}), WaitAfterHit, false);
	}
}

void AEnemy::Die()
{
	EnemyState = EEnemyState::EES_Dead;

	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && DeathMontage != nullptr)
	{
		PlayDeathMontage(true);
	}

	DisplayHPBar(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.f);
}

void AEnemy::Destroyed()
{
	if (IsEquipWeapon())
	{
		for (AWeapon* equippedWeapon : EquippedWeapons)
		{
			equippedWeapon->Destroy();
		}
	}
}
void AEnemy::GetHit(float DamageAmount, const FVector& ImpactPoint, AActor* DamageCauser, AController* EventInstigator)
{
	if (EnemyState == EEnemyState::EES_Dead) return;

	if (EventInstigator != nullptr && EventInstigator->GetPawn() != nullptr)
	{
		CombatTarget = EventInstigator->GetPawn();
	}

	if (Attributes != nullptr)
	{
		Attributes->GetDamage(DamageAmount);
	}

	AfterGetHit(ImpactPoint);
}

//----------------------------------------------------------------- HP 표시
bool AEnemy::DisplayHPBar(bool Display)
{
	if (HPBarComponent == nullptr) { return false; }

	if (Display) 
	{ 
		HPBarComponent->SetVisibility(true); 
	}
	else 
	{ 
		HPBarComponent->SetVisibility(false); 
	}

	return true;
}