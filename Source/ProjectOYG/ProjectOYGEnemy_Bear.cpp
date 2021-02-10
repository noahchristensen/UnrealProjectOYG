// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGEnemy_Bear.h"
#include "ProjectOYGHitBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectOYGEnemyHitBox.h"
#include "ProjectOYGRoarSphere.h"
#include "ProjectOYGAIController.h"
//#include "Camera/CameraShake.h"
#include "Camera/CameraTypes.h"


AProjectOYGEnemy_Bear::AProjectOYGEnemy_Bear() 
{
    //GetCapsuleComponent()->InitCapsuleSize(40.f, 60.0f);

    LeftPawHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("LeftPawHitBox"));
	LeftPawHitBox->SetupAttachment(GetMesh(), TEXT("LeftPawSocket"));
    //LeftPawHitBox->RegisterComponent();
	LeftPawHitBox->SetMobility(EComponentMobility::Movable);
    LeftPawHitBox->SetPoiseDmg(SwipePoiseDamage);

    RightPawHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("RightPawHitBox"));
	RightPawHitBox->SetupAttachment(GetMesh(), TEXT("RightPawSocket"));
    //RightPawHitBox->RegisterComponent();
	RightPawHitBox->SetMobility(EComponentMobility::Movable);
    RightPawHitBox->SetPoiseDmg(SwipePoiseDamage);

    HeadHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("HeadHitBox"));
	HeadHitBox->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
    //HeadHitBox->RegisterComponent();
	HeadHitBox->SetMobility(EComponentMobility::Movable);
    HeadHitBox->SetPoiseDmg(HeadPoiseDamage);

    DiveHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("DiveHitBox"));
	DiveHitBox->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
    //DiveHitBox->RegisterComponent();
	DiveHitBox->SetMobility(EComponentMobility::Movable);
    DiveHitBox->SetPoiseDmg(DivePoiseDamage);

    FullBodyHitBox = CreateDefaultSubobject<UProjectOYGEnemyHitBox>(TEXT("FullBodyHitBox"));
	FullBodyHitBox->SetupAttachment(GetMesh(), TEXT("BodySocket"));
    //FullBodyHitBox->RegisterComponent();
	FullBodyHitBox->SetMobility(EComponentMobility::Movable);
    FullBodyHitBox->SetPoiseDmg(FullBodyPoiseDamage);

    RoarHitSphere = CreateDefaultSubobject<UProjectOYGRoarSphere>(TEXT("RoarHitSphere"));
	RoarHitSphere->SetupAttachment(GetMesh(), TEXT("BodySocket"));
    //RoarHitSphere->RegisterComponent();
	RoarHitSphere->SetMobility(EComponentMobility::Movable);

    RunSpeed = 480.f;
    WalkSpeed = 200.f;

    SwipeDamage = 0.f;
    SwipePoiseDamage = 0.f;
    OneSwipeStartDuration = 0.f;
    OneSwipeAttackDuration = 0.f;
    OneSwipeEndDuration = 0.f;

    TwoSwipeStartDuration = 0.f;
    TwoSwipeAttackDuration = 0.f;
    TwoSwipeTranistionDuration = 0.f;
    TwoSwipeAtttackTwoDuration = 0.f;
    TwoSwipeEndDuration = 0.f;

    HeadDamage = 0.f;
    HeadPoiseDamage = 0.f;
    HeadStartDuration = 0.f;
    HeadAttackDuration = 0.f;
    HeadEndDuration = 0.f;

    FullBodyPoiseDamage = 0.f;

    DiveDamage = 0.f;
    DivePoiseDamage = 0.f;
    DiveLaunchMagnitude = 0.f;
    DiveStartDuration = 0.f;
    DiveAttackDuration = 0.f;
    DiveEndDuration = 0.f;

    JumpDamage = 0.f;
    JumpStartDuration = 0.f;
    JumpLandDuration = 0.f;

    RoarTickDamage = 0.f;
    RoarStartDuration = 0.f;
    RoarAttackDuration = 0.f;
    RoarEndDuration = 0.f;

    HasJumped = false;
}

void AProjectOYGEnemy_Bear::BeginPlay() 
{
    Super::BeginPlay();

    LeftPawHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("LeftPawSocket"));
    RightPawHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("RightPawSocket"));
    HeadHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HeadSocket"));
    DiveHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HeadSocket"));
    FullBodyHitBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("BodySocket"));
    RoarHitSphere->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("BodySocket"));
}

void AProjectOYGEnemy_Bear::Tick(float DeltaTime) 
{
    Super::Tick(DeltaTime);

    if(HasJumped)
    {
        if(!GetMovementComponent()->IsFalling())
        {
            UE_LOG(LogTemp, Warning, TEXT("Bear Landed"));
            JumpEnd();
        }
        else if(GetVelocity().Z < -1.f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Bear Falling"));
            FullBodyHitBox->DeactivateHitBox();
            FullBodyHitBox->ActivateHitBox(JumpDamage);
        }
    }
}

void AProjectOYGEnemy_Bear::StopAttack() 
{
    Super::StopAttack();
    SetIsAttacking(0); 
    UE_LOG(LogTemp, Warning, TEXT("Bear Hit Stopped"))

    LeftPawHitBox->DeactivateHitBox();
    RightPawHitBox->DeactivateHitBox();
    HeadHitBox->DeactivateHitBox();
    FullBodyHitBox->DeactivateHitBox();
    //GetCharacterMovement()->bOrientRotationToMovement = false;

    GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AProjectOYGEnemy_Bear::StopAllActions() 
{
    Super::StopAllActions();
    StopAttack();

    GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void AProjectOYGEnemy_Bear::Die() 
{
    Super::Die();

    ShouldRespawn = false;
}

void AProjectOYGEnemy_Bear::OneSwipeStart() 
{
    if(GetIsAttacking() > 0){return;}

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 0.f;

    SetIsAttacking(1);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::OneSwipeAttack, OneSwipeStartDuration, false);
}

void AProjectOYGEnemy_Bear::OneSwipeAttack() 
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    UE_LOG(LogTemp, Warning, TEXT("Bear OneSwipe Active"));
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::OneSwipeEnd, OneSwipeAttackDuration, false);
    RightPawHitBox->ActivateHitBox(SwipeDamage);
}

void AProjectOYGEnemy_Bear::OneSwipeEnd() 
{
    UE_LOG(LogTemp, Warning, TEXT("Bear OneSwipe Inactive"));
    RightPawHitBox->DeactivateHitBox();
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::StopAttack, OneSwipeEndDuration, false);
}

void AProjectOYGEnemy_Bear::TwoSwipeStart() 
{
    if(GetIsAttacking() > 0){return;}

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 0.f;

    SetIsAttacking(2);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::TwoSwipeAttack, TwoSwipeStartDuration, false);
}

void AProjectOYGEnemy_Bear::TwoSwipeAttack() 
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    UE_LOG(LogTemp, Warning, TEXT("Bear TwoSwipe One Active"));
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::TwoSwipeTransition, TwoSwipeAttackDuration, false);
    RightPawHitBox->ActivateHitBox(SwipeDamage);
}

void AProjectOYGEnemy_Bear::TwoSwipeTransition() 
{
    UE_LOG(LogTemp, Warning, TEXT("Bear TwoSwipe One Inactive"));
    RightPawHitBox->DeactivateHitBox();
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::TwoSwipeAttackTwo, TwoSwipeTranistionDuration, false);
}

void AProjectOYGEnemy_Bear::TwoSwipeAttackTwo() 
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    UE_LOG(LogTemp, Warning, TEXT("Bear TwoSwipe Two Active"));
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::TwoSwipeEnd, TwoSwipeAttackDuration, false);
    LeftPawHitBox->ActivateHitBox(SwipeDamage);
}

void AProjectOYGEnemy_Bear::TwoSwipeEnd() 
{
    UE_LOG(LogTemp, Warning, TEXT("Bear TwoSwipe Two InActive"));
    LeftPawHitBox->DeactivateHitBox();
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::StopAttack, TwoSwipeEndDuration, false);
}

void AProjectOYGEnemy_Bear::HeadStart() 
{
    if(GetIsAttacking() > 0){return;}

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 0.f;

    SetIsAttacking(3);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::HeadAttack, HeadStartDuration, false);
}

void AProjectOYGEnemy_Bear::HeadAttack() 
{
    GetCharacterMovement()->bOrientRotationToMovement = true;

    UE_LOG(LogTemp, Warning, TEXT("Bear Head Active"));
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::HeadEnd, HeadAttackDuration, false);
    HeadHitBox->ActivateHitBox(HeadDamage);
}

void AProjectOYGEnemy_Bear::HeadEnd() 
{
    UE_LOG(LogTemp, Warning, TEXT("Bear Head Inactive"));
    HeadHitBox->DeactivateHitBox();
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::StopAttack, HeadEndDuration, false);
}

void AProjectOYGEnemy_Bear::DiveStart() 
{
    if(GetIsAttacking() > 0){return;}

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 0.f;

    SetIsAttacking(4);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::DiveAttack, HeadStartDuration, false);
}

void AProjectOYGEnemy_Bear::DiveAttack() 
{
    GetCharacterMovement()->bOrientRotationToMovement = true;
    LaunchCharacter(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0.3).GetSafeNormal() * DiveLaunchMagnitude, true, true);

    UE_LOG(LogTemp, Warning, TEXT("Bear Dive Active"));
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::DiveEnd, DiveAttackDuration, false);
    DiveHitBox->ActivateHitBox(DiveDamage);
}

void AProjectOYGEnemy_Bear::DiveEnd() 
{
    UE_LOG(LogTemp, Warning, TEXT("Bear Dive Inactive"));
    DiveHitBox->DeactivateHitBox();
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::StopAttack, DiveEndDuration, false);
}

void AProjectOYGEnemy_Bear::JumpStart() 
{
    if(GetIsAttacking() > 0){return;}

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 0.f;

    SetIsAttacking(5);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::JumpAction, JumpStartDuration, false);
}

void AProjectOYGEnemy_Bear::JumpAction() 
{
    Jump();
    FullBodyHitBox->ActivateHitBox(JumpDamage * 0.2f);
    GetCharacterMovement()->bOrientRotationToMovement = true;
    
    HasJumped = true;
}

void AProjectOYGEnemy_Bear::JumpEnd() 
{
    HasJumped = false;
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::StopAttack, JumpLandDuration, false);
    FullBodyHitBox->DeactivateHitBox();
}

void AProjectOYGEnemy_Bear::RoarStart() 
{
    if(GetIsAttacking() > 0){return;}

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->MaxWalkSpeed = 0.f;

    SetIsAttacking(6);
    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::RoarAttack, RoarStartDuration, false);
}

void AProjectOYGEnemy_Bear::RoarAttack() 
{
    UE_LOG(LogTemp, Warning, TEXT("Bear Roar Active"));
    GetCharacterMovement()->bOrientRotationToMovement = true;

    RoarHitSphere->ActivateHitBox(RoarTickDamage);
    float RoarScale = 100.f / DistanceFromPlayer();

    APlayerController* Player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    //Player->ClientPlayCameraShake(RoarShakeClass, RoarScale);

    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::RoarEnd, RoarAttackDuration, false);
}

void AProjectOYGEnemy_Bear::RoarEnd() 
{
    UE_LOG(LogTemp, Warning, TEXT("Bear Roar Inactive"));
    RoarHitSphere->DeactivateHitBox();
    APlayerController* Player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    //Player->ClientStopCameraShake(RoarShakeClass, false);

    GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGEnemy_Bear::StopAttack, RoarEndDuration, false);
}