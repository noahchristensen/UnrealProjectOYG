// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGEnemy_Slug.h"
#include "ProjectOYGHitBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectOYGEnemyHitBox.h"


AProjectOYGEnemy_Slug::AProjectOYGEnemy_Slug() 
{
    GetCapsuleComponent()->InitCapsuleSize(40.f, 60.0f);

    HeadHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("HeadHitBox"));
	HeadHitBox->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
    //HeadHitBox->RegisterComponent();
	HeadHitBox->SetMobility(EComponentMobility::Movable);
    HeadHitBox->SetPoiseDmg(StrikePoiseDmg);

    StrikeLaunchMagnitude = 100.f;
    StrikeAttackDamage = 35.f;
    StrikeStartUp = 0.8f;
    StrikeHitDuration = 0.5f;
    SpitStartUp = 0.5f;
    SpitPoiseDmg = 1.f;
}

void AProjectOYGEnemy_Slug::BeginPlay() 
{
    Super::BeginPlay();

    HeadHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HeadSocket"));
}

void AProjectOYGEnemy_Slug::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
}

void AProjectOYGEnemy_Slug::StopAttack() 
{
    Super::StopAttack();
    SetIsAttacking(0); 
    UE_LOG(LogTemp, Warning, TEXT("Slug Hit Inactive"));

    HeadHitBox->DeactivateHitBox();
    GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AProjectOYGEnemy_Slug::StopAllActions() 
{
    Super::StopAllActions();
    StopAttack();
    UE_LOG(LogTemp, Warning, TEXT("Slug stop all called"));

    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AProjectOYGEnemy_Slug::StrikeStart() 
{
    SetIsAttacking(1);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Slug::StrikeAttack, StrikeStartUp, false);
}

void AProjectOYGEnemy_Slug::StrikeAttack() 
{
    UE_LOG(LogTemp, Warning, TEXT("Slug Hit Active"));
    GetCharacterMovement()->bOrientRotationToMovement = true;

    LaunchCharacter(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0).GetSafeNormal() * StrikeLaunchMagnitude, true, true);
    //Jump();

    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Slug::StopAttack, StrikeHitDuration, false);
    HeadHitBox->ActivateHitBox(StrikeAttackDamage);
}

void AProjectOYGEnemy_Slug::SpitStart() 
{
    SetIsAttacking(2);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Slug::SpitAttack, SpitStartUp, false);
}

void AProjectOYGEnemy_Slug::SpitAttack() 
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    UE_LOG(LogTemp, Warning, TEXT("Spit attack attempted"));

    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Slug::StopAttack, SpitEndLag, false);

    // Attempt to fire a projectile.
    if (ProjectileClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("projectile class fulfilled"));

        MuzzleOffset.Set(65.0f, 0.0f, -30.0f);

        FRotator MuzzleRotation = GetActorRotation();

        // Transform MuzzleOffset from camera space to world space.
        FVector MuzzleLocation = GetActorLocation() + FTransform(MuzzleRotation).TransformVector(MuzzleOffset);

        MuzzleRotation.Pitch += 15.0f;

        UWorld* World = GetWorld();
        if (World)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = this;
            SpawnParams.Instigator = GetInstigator();

            // Spawn the projectile at the muzzle.
            AProjectOYGProjectile* Projectile = World->SpawnActor<AProjectOYGProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
            if (Projectile != nullptr)
            {
                // Set the projectile's initial trajectory.
                Projectile->FireInDirection(Projectile->GetActorForwardVector());
                Projectile->SetPoiseDmg(SpitPoiseDmg);
            }
        }
    }
}
