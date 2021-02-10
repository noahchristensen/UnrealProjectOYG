// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOYGEnemyTemp.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectOYGWeapon.h"

// Sets default values
AProjectOYGEnemyTemp::AProjectOYGEnemyTemp()
{
	GetCapsuleComponent()->InitCapsuleSize(22.f, 90.0f);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LockOnVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockOnVisual"));
	//LockOnVisual->RegisterComponent();
	LockOnVisual->SetupAttachment(RootComponent);
	LockOnVisual->SetMobility(EComponentMobility::Movable);
	LockOnVisual->AddLocalRotation(FRotator(180.f, 0.f, 90.f), false, 0, ETeleportType::None);

	BackStabCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BackStabCollider"));
	//BackStabCollider->RegisterComponent();
	BackStabCollider->SetupAttachment(RootComponent);
	BackStabCollider->SetMobility(EComponentMobility::Movable);

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultTurnSpeed = GetCharacterMovement()->RotationRate.Yaw;

	MaxHealth = 100.f;
	Health = MaxHealth;
	IsAttacking = false;
	IsStaggered = false;
	IsBackStabbed = false;
}

// Called when the game starts or when spawned
void AProjectOYGEnemyTemp::BeginPlay()
{
	Super::BeginPlay();

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultTurnSpeed = GetCharacterMovement()->RotationRate.Yaw;
	
	if(WeaponClass)
	{
		Weapon = GetWorld()->SpawnActor<AProjectOYGWeapon>(WeaponClass);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		Weapon->SetOwner(this);
	}
}

// Called every frame
void AProjectOYGEnemyTemp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AProjectOYGEnemyTemp::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AProjectOYGEnemyTemp::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) 
{
	UE_LOG(LogTemp, Warning, TEXT("Initial Health: %f, Damage To be dealt: %f" ), Health, DamageAmount);

	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = FMath::Min(Health, DamageToApply);
	Health = Health - DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("After Damage Health: %f"), Health);

	if(GetIsDead())
	{
		Die();
	}
	else
	{
		StopAllActions();
		UE_LOG(LogTemp, Warning, TEXT("Staggered?"));
		IsStaggered = true;

		GetWorldTimerManager().SetTimer(UnusedHandleTwo, this, &AProjectOYGEnemyTemp::StopStagger, StaggerDuration, false);
	}

	return DamageToApply;
}

void AProjectOYGEnemyTemp::TakeDamageLaunch(FVector LaunchDirection, float LaunchDistance) 
{
	LaunchCharacter(LaunchDirection.GetSafeNormal() * LaunchDistance, true, false);
}

bool AProjectOYGEnemyTemp::GetIsDead() const
{
	if(Health <= 0)
	{
		return true;
	}
	return false;
}

float AProjectOYGEnemyTemp::GetHealthPercent() const
{
	return Health / MaxHealth;
}

int AProjectOYGEnemyTemp::GetIsAttacking() const
{
	return IsAttacking;
}

bool AProjectOYGEnemyTemp::GetIsBackStabbed() const
{
	return IsBackStabbed;
}

bool AProjectOYGEnemyTemp::IsBackStabable() 
{
	return true;
}

void AProjectOYGEnemyTemp::BackStabFreeze(float BackStabTime) 
{
	StopAllActions();
	IsBackStabbed = true;
	IsStaggered = true;
	GetCharacterMovement()->RotationRate.Yaw = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGEnemyTemp::BackStabUnFreeze, BackStabTime, false);
}

void AProjectOYGEnemyTemp::BackStabUnFreeze() 
{
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->RotationRate.Yaw = DefaultTurnSpeed;
	StopStagger();
	IsBackStabbed = false;
}

float AProjectOYGEnemyTemp::BackStabTakeDamage(float DamageAmount) 
{
	float DamageToApply = FMath::Min(Health, DamageAmount);
	Health = Health - DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("After Damage Health: %f"), Health);

	if(GetIsDead())
	{
		Die();
	}

	return DamageToApply;
}

float AProjectOYGEnemyTemp::DistanceFromPlayer() 
{
	if(GetWorld())
	{
		if(GetWorld()->GetFirstPlayerController())
		{
			if(GetWorld()->GetFirstPlayerController()->GetPawn())
			{
				FVector PlayerCharacterLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
				return FVector::Dist(this->GetActorLocation(), PlayerCharacterLoc);
			}
		}
	}

	return 500000000.f;
}

void AProjectOYGEnemyTemp::TargetLockOn() 
{
	LockOnVisual->SetHiddenInGame(false, false);
}

void AProjectOYGEnemyTemp::TargetLockOff() 
{
	LockOnVisual->SetHiddenInGame(true, false);
}

bool AProjectOYGEnemyTemp::GetIsStaggered() const
{
	return IsStaggered;
}

void AProjectOYGEnemyTemp::LightAttack() 
{
	if(!PerformingAction())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;

		IsAttacking = 1;
		Weapon->StartAttack(1, false);

		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGEnemyTemp::StopAttack, Weapon->GetLightAttackDuration(), false);
	}
}

void AProjectOYGEnemyTemp::HeavyAttack() 
{
	if(!PerformingAction())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;

		IsAttacking = 2;
		Weapon->StartAttack(2, false);

		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGEnemyTemp::StopAttack, Weapon->GetHeavyAttackDuration(), false);
	}
}

void AProjectOYGEnemyTemp::StopAttack() 
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	//bUseControllerRotationYaw = true;

	IsAttacking = 0;

	UE_LOG(LogTemp, Warning, TEXT("stop attacking"));
}

void AProjectOYGEnemyTemp::StopStagger() 
{
	IsStaggered = false;
}

void AProjectOYGEnemyTemp::StopAllActions() 
{
	if(Weapon != nullptr)
	{
		if(IsAttacking != 0)
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(Weapon);
			Weapon->DeactivateHitbox();
			StopAttack();
		}
	}
}

bool AProjectOYGEnemyTemp::PerformingAction() 
{
	if(IsAttacking != 0 || GetIsDead() || IsStaggered)
	{
		return true;
	}
	return false;
}

void AProjectOYGEnemyTemp::Die() 
{
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->RotationRate.Yaw = 0.f;
	DetachFromControllerPendingDestroy();
	//GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BackStabCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


