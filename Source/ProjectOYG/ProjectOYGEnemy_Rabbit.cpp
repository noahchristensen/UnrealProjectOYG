// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGEnemy_Rabbit.h"
#include "ProjectOYGHitBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectOYGEnemyHitBox.h"


AProjectOYGEnemy_Rabbit::AProjectOYGEnemy_Rabbit() 
{
    GetCapsuleComponent()->InitCapsuleSize(40.f, 60.0f);

    FeetHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("FeetHitBox"));
	FeetHitBox->SetupAttachment(GetMesh(), TEXT("BackFootSocket"));
    //FeetHitBox->RegisterComponent();
	FeetHitBox->SetMobility(EComponentMobility::Movable);
    FeetHitBox->SetPoiseDmg(KickPoiseDmg);

    HornHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("HornHitBox"));
	HornHitBox->SetupAttachment(GetMesh(), TEXT("HornSocket"));
    //HornHitBox->RegisterComponent();
	HornHitBox->SetMobility(EComponentMobility::Movable);
    HornHitBox->SetPoiseDmg(JumpPoiseDmg);

    KickAttackDamage = 20.f;
    JumpAttackDamage = 30.f;
    KickStartUp = 0.5f;
    KickHitDuration = 0.5f;
    JumpStartUp = 0.2f;
    JumpHitDuration = 0.6f;
    JumpMagnitude = 30.f;
}

void AProjectOYGEnemy_Rabbit::BeginPlay() 
{
    Super::BeginPlay();

    FeetHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("BackFootSocket"));
    HornHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HornSocket"));
}

void AProjectOYGEnemy_Rabbit::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);
}

void AProjectOYGEnemy_Rabbit::StopAttack() 
{
    Super::StopAttack();
    SetIsAttacking(0); 
    UE_LOG(LogTemp, Warning, TEXT("Rabbit Hit Inactive"));

    FeetHitBox->DeactivateHitBox();
    HornHitBox->DeactivateHitBox();
    GetCharacterMovement()->bOrientRotationToMovement = false;
}

void AProjectOYGEnemy_Rabbit::StopAllActions() 
{
    Super::StopAllActions();
    StopAttack();

    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AProjectOYGEnemy_Rabbit::JumpStart() 
{
    SetIsAttacking(1);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Rabbit::JumpAttack, JumpStartUp, false);
}

void AProjectOYGEnemy_Rabbit::JumpAttack() 
{
    UE_LOG(LogTemp, Warning, TEXT("Rabbit Hit Active"));
    GetCharacterMovement()->bOrientRotationToMovement = true;

    LaunchCharacter(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.7).GetSafeNormal() * JumpMagnitude, true, true);
    //Jump();

    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Rabbit::StopAttack, JumpHitDuration, false);
    HornHitBox->ActivateHitBox(JumpAttackDamage);
}

void AProjectOYGEnemy_Rabbit::KickFrontStart() 
{
    SetIsAttacking(2);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Rabbit::KickFrontAttack, KickStartUp, false);
}

void AProjectOYGEnemy_Rabbit::KickFrontAttack() 
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    UE_LOG(LogTemp, Warning, TEXT("Rabbit Hit Active"));
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Rabbit::StopAttack, KickHitDuration, false);
    FeetHitBox->ActivateHitBox(KickAttackDamage);
}