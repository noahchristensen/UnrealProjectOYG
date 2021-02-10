// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGHitBox.h"
#include "ProjectOYGCharacter.h"
#include "Components/BoxComponent.h"

AProjectOYGHitBox::AProjectOYGHitBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Active = false;
	HasHit = false;
	HitBoxDamage = false;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	//Collider->RegisterComponent();
	Collider->SetupAttachment(RootComponent);
	Collider->SetMobility(EComponentMobility::Movable);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &AProjectOYGHitBox::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AProjectOYGHitBox::OnOverlapEnd);
}

void AProjectOYGHitBox::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProjectOYGHitBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Active = false;
	HasHit = false;
}

void AProjectOYGHitBox::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if(Active && !HasHit && OtherActor->IsA(AProjectOYGCharacter::StaticClass()))
	{
		APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if(OwnerPawn == nullptr){return;}
		AController* OwnerController = OwnerPawn->GetController();

		FPointDamageEvent DamageEvent; // nothing for now, can add for different hit effects possibly

		OtherActor->TakeDamage(HitBoxDamage, DamageEvent, OwnerController, this);
		HasHit = true;  // could just set active to false here
	}
}

void AProjectOYGHitBox::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	// uhh
}

void AProjectOYGHitBox::ActivateHitBox(float Time, float Damage) 
{
	Active = true;
	HitBoxDamage = Damage;

	GetWorldTimerManager().SetTimer(DeactivationHandle, this, &AProjectOYGHitBox::DeactivateHitBox, Time, false);
}

void AProjectOYGHitBox::DeactivateHitBox() 
{
	Active = false;
	HasHit = false;
}

