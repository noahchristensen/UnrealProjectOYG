// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGEnemy_Humanoid.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectOYGWeapon.h"

AProjectOYGEnemy_Humanoid::AProjectOYGEnemy_Humanoid()
{
	//Super::AProjectOYGEnemy();

    BackStabCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BackStabCollider"));
	//BackStabCollider->RegisterComponent();
	BackStabCollider->SetupAttachment(RootComponent);
	BackStabCollider->SetMobility(EComponentMobility::Movable);

	IsBackStabbed = false;
}

void AProjectOYGEnemy_Humanoid::BeginPlay() 
{
    Super::BeginPlay();

	BackStabCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

    if(WeaponClass)
	{
		Weapon = GetWorld()->SpawnActor<AProjectOYGWeapon>(WeaponClass);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		Weapon->SetOwner(this);
	}
}

void AProjectOYGEnemy_Humanoid::Destroyed() 
{
	Super::Destroyed();

	if(Weapon != nullptr)
	{
		Weapon->Destroy();
	}
}

void AProjectOYGEnemy_Humanoid::LightAttack() 
{
	if(!PerformingAction())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;

		SetIsAttacking(1);
		Weapon->StartAttack(1, false);

		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGEnemy_Humanoid::StopAttack, Weapon->GetLightAttackDuration(), false);
	}
}

void AProjectOYGEnemy_Humanoid::HeavyAttack() 
{
	if(!PerformingAction())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;

		SetIsAttacking(2);
		Weapon->StartAttack(2, false);

		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGEnemy_Humanoid::StopAttack, Weapon->GetHeavyAttackDuration(), false);
	}
}

void AProjectOYGEnemy_Humanoid::StopAttack() 
{
	GetCharacterMovement()->bOrientRotationToMovement = false;
	//bUseControllerRotationYaw = true;

	SetIsAttacking(0);

	UE_LOG(LogTemp, Warning, TEXT("stop attacking"));
}

bool AProjectOYGEnemy_Humanoid::GetIsBackStabbed() const
{
	return IsBackStabbed;
}

bool AProjectOYGEnemy_Humanoid::IsBackStabable() 
{
	return true;
}

void AProjectOYGEnemy_Humanoid::BackStabFreeze(float BackStabTime) 
{
	StopAllActions();
	IsBackStabbed = true;
	SetIsStaggered(true);
	GetCharacterMovement()->RotationRate.Yaw = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGEnemy_Humanoid::BackStabUnFreeze, BackStabTime, false);
}

void AProjectOYGEnemy_Humanoid::BackStabUnFreeze() 
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
	GetCharacterMovement()->MaxWalkSpeed = GetDefaultWalkSpeed();
	GetCharacterMovement()->RotationRate.Yaw = GetDefaultTurnSpeed();
	StopStagger();
	IsBackStabbed = false;
}

float AProjectOYGEnemy_Humanoid::BackStabTakeDamage(float DamageAmount) 
{
	float DamageToApply = FMath::Min(GetHealth(), DamageAmount);
	SetHealth(GetHealth() - DamageToApply);
	UE_LOG(LogTemp, Warning, TEXT("After Damage Health: %f"), GetHealth());

	if(GetIsDead())
	{
		Die();
	}

	return DamageToApply;
}

void AProjectOYGEnemy_Humanoid::StopAllActions() 
{
    Super::StopAllActions();

	if(Weapon != nullptr)
	{
		if(GetIsAttacking() != 0)
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(Weapon);
			Weapon->DeactivateHitbox();
			StopAttack();
		}
	}
}