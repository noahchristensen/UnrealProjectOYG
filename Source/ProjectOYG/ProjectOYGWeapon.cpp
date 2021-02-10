// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGWeapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectOYGCharacter.h"
#include "ProjectOYGEnemy.h"


// Sets default values
AProjectOYGWeapon::AProjectOYGWeapon()
{
	

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	MainCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MainCollision"));
	MainCollision->SetupAttachment(Mesh);

	static ConstructorHelpers::FObjectFinder<USoundCue> AttackSoundCueObject(TEXT("SoundCue'/Game/Audio/SwordSoundCue.SwordSoundCue'"));
	if(AttackSoundCueObject.Succeeded())
	{
		AttackSoundCue = AttackSoundCueObject.Object;

		AttackAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AttackAudioComponent"));
		AttackAudioComponent->SetupAttachment(RootComponent);
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> HitSoundCueObject(TEXT("SoundCue'/Game/Audio/HitSoundCue.HitSoundCue'"));
	if(HitSoundCueObject.Succeeded())
	{
		HitSoundCue = HitSoundCueObject.Object;

		HitAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HitAudioComponent"));
		HitAudioComponent->SetupAttachment(RootComponent);
	}

	LightAttackStaminaCost = 20.f;
	HeavyAttackStaminaCost = 35.f;
	LightAttackStartup = 0.1f;
	HeavyAttackStartup = 0.15f;
	LightAttackHitDuration = 0.2f;
	HeavyAttackHitDuration = 0.4f;
	LightAttackDuration = 0.5f;
	HeavyAttackDuration = 0.8f;
	ComboLightAttackStartup = 0.1f;
	ComboLightAttackHitDuration = 0.2f;
	ComboLightAttackDuration = 0.5f;
	ComboEndBufferDuration = 0.2f;
	FallingAttackStartup = 0.4f;
	BackStabDuration = 2.f;
	BackStabDistance = 50.f;
	BackStabHitTime = 1.2f;
	BackStabDamage = 100.f;
	ComboNum = 0;
	LightAtkDamage = 20.f;
	HeavyAtkDamage = 30.f;
	FallingAtkDamage = 40.f;
	LightLaunchDistance = 10.f;
	HeavyLaunchDistance = 15.f;
	FallLaunchDistance = 15.f;
	Damage = 0.f;
	LaunchDistance = 0.f;
	PoiseDmg = 0.f;
	SavedAtkID = 0;
	WeaponLength = 30.f;

	TypeID = 1;

	Combos = false;

	HitBoxActive = false;
	IsCombo = false;
	IsFallingAtk = false;
}

// Called when the game starts or when spawned
void AProjectOYGWeapon::BeginPlay()
{
	Super::BeginPlay();

	if(AttackAudioComponent && AttackSoundCue)
	{
		AttackAudioComponent->SetSound(AttackSoundCue);
	}

	if(HitAudioComponent && HitSoundCue)
	{
		HitAudioComponent->SetSound(HitSoundCue);
	}
}

// Called every frame
void AProjectOYGWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(HitBoxActive)
	{
		Attacking();
	}

	AProjectOYGCharacter* Player = Cast<AProjectOYGCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if(Player == nullptr){return;}
	if(!Player->GetCharacterMovement()->IsFalling() && IsFallingAtk)
	{
		DeactivateHitbox();
	}
}

float AProjectOYGWeapon::GetLightAttackStaminaCost() 
{
	return LightAttackStaminaCost;
}

float AProjectOYGWeapon::GetHeavyAttackStaminaCost() 
{
	return HeavyAttackStaminaCost;
}

float AProjectOYGWeapon::GetLightAttackDuration() 
{
	if(IsCombo)
	{
		return ComboLightAttackDuration;
	}
	return LightAttackDuration;
}

float AProjectOYGWeapon::GetComboEndBufferDuration() 
{
	return ComboEndBufferDuration;
}

float AProjectOYGWeapon::GetHeavyAttackDuration() 
{
	return HeavyAttackDuration;
}

float AProjectOYGWeapon::GetBackStabDuration() 
{
	return BackStabDuration;
}

float AProjectOYGWeapon::GetBackStabDistance() 
{
	return BackStabDistance;
}

float AProjectOYGWeapon::GetBackStabHitTime() 
{
	return BackStabHitTime;
}

float AProjectOYGWeapon::GetBackStabDamage() 
{
	return BackStabDamage;
}

void AProjectOYGWeapon::StartAttack(int AtkID, bool Combo) 
{

	switch(AtkID)
	{
		case 0:
			Damage = 0.f;
			break;
		case 1:
			Damage = LightAtkDamage;
			LaunchDistance = LightLaunchDistance;
			IsCombo = Combo;
			if(IsCombo)
			{
				ComboNum++;
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGWeapon::ActivateHitbox, ComboLightAttackStartup, false);
			}
			else
			{
				ComboNum = 0;
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGWeapon::ActivateHitbox, LightAttackStartup, false);
			}
			break;
		case 2:
			Damage = HeavyAtkDamage;
			LaunchDistance = HeavyLaunchDistance;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGWeapon::ActivateHitbox, HeavyAttackStartup, false);
			break;
		case 3:
			Damage = FallingAtkDamage;
			LaunchDistance = FallLaunchDistance;
			IsFallingAtk = true;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGWeapon::ActivateHitbox, FallingAttackStartup, false);
			break;
		//case 5: Backstab - handled in character
	}
	SavedAtkID = AtkID;
}

void AProjectOYGWeapon::ActivateHitbox() 
{
	if(AttackAudioComponent && !AttackAudioComponent->IsPlaying())
	{
		AttackAudioComponent->SetPitchMultiplier(FMath::RandRange(0.7f, 0.9f));
		AttackAudioComponent->Play(0.f);
	}

	AProjectOYGCharacter* Player = Cast<AProjectOYGCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if(IsFallingAtk && !Player->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Active"));
	HitBoxActive = true;

	switch(SavedAtkID)
	{
		case 1:
			if(IsCombo)
			{
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGWeapon::DeactivateHitbox, ComboLightAttackHitDuration, false);
			}
			else
			{
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGWeapon::DeactivateHitbox, LightAttackHitDuration, false);
			}
			break;
		case 2:
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGWeapon::DeactivateHitbox, HeavyAttackHitDuration, false);
			break;
		case 3:
			if(IsFallingAtk != true)
			{
				HitBoxActive = false;
			}
			break;
	}
}

void AProjectOYGWeapon::DeactivateHitbox() 
{
	UE_LOG(LogTemp, Warning, TEXT("DeActive"));
	HitBoxActive = false;
	IsFallingAtk = false;
	for (int i = 0; i < EnemyHitArr.Num(); i++) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *EnemyHitArr[i]->GetName());
   		//Actor = EnemyHitArr[i];
 	}

	EnemyHitArr.Empty();
}

int AProjectOYGWeapon::GetWeaponTypeID() 
{
	return TypeID;
}

bool AProjectOYGWeapon::GetCombos() 
{
	return Combos;
}

float AProjectOYGWeapon::GetPoiseDmg() 
{
	return PoiseDmg;
}

bool AProjectOYGWeapon::GetIsComboSecondHit() 
{
	if(ComboNum % 2 == 0) // even number, so first hit
	{
		return false;
	}
	return true;
}

void AProjectOYGWeapon::Attacking() 
{
	FHitResult OutHit;
	FVector Start = Mesh->GetComponentLocation();

	// alternatively you can get the camera location
	// FVector Start = FirstPersonCameraComponent->GetComponentLocation();

	FVector ForwardVector = -Mesh->GetRightVector();
	FVector End = (Start + (ForwardVector * WeaponLength));
	FCollisionQueryParams CollisionParams;

	CollisionParams.AddIgnoredActor(this);
	if(GetOwner()->IsA(AProjectOYGEnemy::StaticClass()))
	{
		for (TObjectIterator<AProjectOYGEnemy> Itr; Itr; ++Itr)
		{
			CollisionParams.AddIgnoredActor(*Itr);
		}
	}
	CollisionParams.AddIgnoredActor(GetOwner());

	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);

	if(GetWorld()->LineTraceSingleByChannel(OutHit, End, Start, ECC_WorldDynamic, CollisionParams)) 
	{
		if(OutHit.bBlockingHit)
		{
			// if (GEngine) 
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("You are hitting: %s"), *OutHit.GetActor()->GetName()));
			// 	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Impact Point: %s"), *OutHit.ImpactPoint.ToString()));
			// 	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Normal Point: %s"), *OutHit.ImpactNormal.ToString()));
			// }

			AActor* HitActor = OutHit.GetActor();
			if(HitActor == nullptr) {return;}

			for (int i = 0; i < EnemyHitArr.Num(); i++) 
			{
				if(EnemyHitArr[i] == (HitActor))
				{
					return;
				}
			}

			APawn* OwnerPawn = Cast<APawn>(GetOwner());
			if(OwnerPawn == nullptr){return;}
			AController* OwnerController = OwnerPawn->GetController();
			if(OwnerController == nullptr){return;}
			FPointDamageEvent DamageEvent(Damage, OutHit, ForwardVector, nullptr);

			if((HitActor->IsA(AProjectOYGCharacter::StaticClass())))
			{
				// Used for poise calculation - determine whether character is staggered
				Cast<AProjectOYGCharacter>(HitActor)->SetPoiseDmgTaken(PoiseDmg);
			}
			else if((HitActor->IsA(AProjectOYGEnemy::StaticClass())))
			{
				// Used for poise calculation - determine whether enemy is staggered
				Cast<AProjectOYGEnemy>(HitActor)->SetPoiseDmgTaken(PoiseDmg);

				// Launch enemy away from when hit
				FVector LaunchDirection = UKismetMathLibrary::GetDirectionUnitVector(GetOwner()->GetActorLocation(), HitActor->GetActorLocation());
				Cast<AProjectOYGEnemy>(HitActor)->TakeDamageLaunch(LaunchDirection, LaunchDistance);

				Cast<AProjectOYGEnemy>(HitActor)->Bleed(OutHit.Location, OutHit.ImpactNormal.Rotation());
			}

			if(HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this) >= 0)
			{
				// Enemy added to array cannot be hit again
				EnemyHitArr.Add(HitActor);

				if(HitAudioComponent)
				{
					HitAudioComponent->SetPitchMultiplier(FMath::RandRange(0.8f, 1.f));
					HitAudioComponent->Play(0.f);
				}
			}
		}
	}
}
