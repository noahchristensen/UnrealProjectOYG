// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectOYGEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectOYGWeapon.h"

// Sets default values
AProjectOYGEnemy::AProjectOYGEnemy()
{
	GetCapsuleComponent()->InitCapsuleSize(22.f, 90.0f);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LockOnVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LockOnVisual"));
	LockOnVisual->SetupAttachment(GetMesh());
	//LockOnVisual->RegisterComponent();
	LockOnVisual->SetMobility(EComponentMobility::Movable);
	LockOnVisual->AddLocalRotation(FRotator(180.f, 0.f, 90.f), false, 0, ETeleportType::None);

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultTurnSpeed = GetCharacterMovement()->RotationRate.Yaw;

	LockOnVisual->SetHiddenInGame(false, true);
	LockOnVisual->SetVisibility(false, true);

	MaxHealth = 100.f;
	Health = MaxHealth;
	IsAttacking = 0;
	IsStaggered = false;

	MaxPoise = 0.f;
	CurrentPoise = MaxPoise;
	PoiseRegenAmount = 0.01;
	PoiseDmgTaken = 0.f;

	ShouldRespawn = true;
}

// Called when the game starts or when spawned
void AProjectOYGEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	// must attach to component on begin play to account for respawning the enemy
	LockOnVisual->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	InitialLocation = GetActorLocation();
	InitialRotation = GetActorRotation();

	DefaultWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultTurnSpeed = GetCharacterMovement()->RotationRate.Yaw;
}

// Called every frame
void AProjectOYGEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PoiseRegen();
}

// Called to bind functionality to input
void AProjectOYGEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float AProjectOYGEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) 
{
	UE_LOG(LogTemp, Warning, TEXT("Initial Health: %f, Damage To be dealt: %f" ), Health, DamageAmount);

	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = FMath::Min(Health, DamageToApply);
	Health = Health - DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("After Damage Health: %f"), Health);

	if(GetIsDead())
	{
		StopAllActions();
		Die();
	}
	else
	{
		CurrentPoise = FMath::Max(0.f, CurrentPoise - PoiseDmgTaken);
		UE_LOG(LogTemp, Warning, TEXT("Poise: %f, PoiseDmgTaken: %f"), CurrentPoise, PoiseDmgTaken);
		if(CurrentPoise <= 0.01)
		{
			StopAllActions();
			UE_LOG(LogTemp, Warning, TEXT("Staggered"));
			IsStaggered = true;
			GetCharacterMovement()->RotationRate.Yaw = 0;
			GetCharacterMovement()->bOrientRotationToMovement = true;

			GetWorldTimerManager().SetTimer(UnusedHandleTwo, this, &AProjectOYGEnemy::StopStagger, StaggerDuration, false);

			CurrentPoise = MaxPoise;
		}
		PoiseDmgTaken = 0.f;
	}

	return DamageToApply;
}

void AProjectOYGEnemy::TakeDamageLaunch(FVector LaunchDirection, float LaunchDistance) 
{
	LaunchCharacter(LaunchDirection.GetSafeNormal() * LaunchDistance, true, false);
}

bool AProjectOYGEnemy::GetIsDead() const
{
	if(Health <= 0.f)
	{
		return true;
	}
	return false;
}

float AProjectOYGEnemy::GetHealthPercent() const
{
	return Health / MaxHealth;
}

int AProjectOYGEnemy::GetIsAttacking() const
{
	return IsAttacking;
}

float AProjectOYGEnemy::DistanceFromPlayer() 
{
	if(Health <= 0)
	{
		return 500000000.f;
	}

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

void AProjectOYGEnemy::TargetLockOn() 
{
	LockOnVisual->SetVisibility(true, true);
}

void AProjectOYGEnemy::TargetLockOff() 
{
	LockOnVisual->SetVisibility(false, true);
}

void AProjectOYGEnemy::StopAttack() 
{
	IsAttacking = 0;
}

bool AProjectOYGEnemy::GetIsStaggered() const
{
	return IsStaggered;
}

void AProjectOYGEnemy::StopStagger() 
{
	IsStaggered = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate.Yaw = DefaultTurnSpeed;
}

void AProjectOYGEnemy::StopAllActions() 
{
	StopAttack();
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AProjectOYGEnemy::SetIsStaggered(bool Stagger) 
{
	IsStaggered = Stagger;
}

void AProjectOYGEnemy::SetIsAttacking(int Attack) 
{
	IsAttacking = Attack;
}

void AProjectOYGEnemy::SetHealth(float NewHealth) 
{
	Health = NewHealth;
}

float AProjectOYGEnemy::GetDefaultWalkSpeed() 
{
	return DefaultWalkSpeed;
}

float AProjectOYGEnemy::GetDefaultTurnSpeed() 
{
	return DefaultWalkSpeed;
}

float AProjectOYGEnemy::GetHealth() 
{
	return Health;
}

void AProjectOYGEnemy::Bleed(FVector Location, FRotator Rotation) 
{
	if(ImpactEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Location, Rotation);
	}
}

void AProjectOYGEnemy::SetPoiseDmgTaken(float PoiseDmg) 
{
	PoiseDmgTaken = PoiseDmg;
}

void AProjectOYGEnemy::PoiseRegen() 
{
	if(CurrentPoise < MaxPoise)
	{
		CurrentPoise = FMath::Min(CurrentPoise + PoiseRegenAmount, MaxPoise);
	}
	else if(CurrentPoise > MaxPoise)
	{
		CurrentPoise = MaxPoise;
	}
}

bool AProjectOYGEnemy::PerformingAction() 
{
	if(IsAttacking != 0 || GetIsDead() || IsStaggered)
	{
		return true;
	}
	return false;
}

void AProjectOYGEnemy::Die() 
{
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetCharacterMovement()->RotationRate.Yaw = 0.f;
	DetachFromControllerPendingDestroy();
	//GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TArray <USceneComponent*> SceneComponents;
	GetCapsuleComponent()->GetChildrenComponents(true, SceneComponents);
	for (auto& Component : SceneComponents)
    {
		if(Component->IsA(UPrimitiveComponent::StaticClass()))
		{
			Cast<UPrimitiveComponent>(Component)->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	SetActorTickEnabled(false);
}

FVector AProjectOYGEnemy::GetInitialLocation() 
{
	return InitialLocation;
}

FRotator AProjectOYGEnemy::GetInitialRotation() 
{
	return InitialRotation;
}
