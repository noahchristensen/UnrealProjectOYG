// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGEnemyHitBox.h"
#include "ProjectOYGEnemy.h" 
#include "ProjectOYGCharacter.h" 
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UProjectOYGEnemyHitBox::UProjectOYGEnemyHitBox()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	//Collider->RegisterComponent();
	Collider->SetupAttachment(this);
	//Collider->SetMobility(EComponentMobility::Movable);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &UProjectOYGEnemyHitBox::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &UProjectOYGEnemyHitBox::OnOverlapEnd);

	IsOverlapping = false;
}


// Called when the game starts
void UProjectOYGEnemyHitBox::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UProjectOYGEnemyHitBox::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(ActorToDamage == nullptr){return;}
	if(IsOverlapping && Active && !HasHit && ActorToDamage->IsA(AProjectOYGCharacter::StaticClass()))
	{
		InflictDamage();
	}
}

void UProjectOYGEnemyHitBox::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	//UE_LOG(LogTemp, Warning, TEXT("overlap begin with: "), *OtherActor->GetName());

	if(OtherActor->IsA(AProjectOYGCharacter::StaticClass()))
	{
		ActorToDamage = OtherActor;
		IsOverlapping = true;
	}

	if(Active && !HasHit && OtherActor->IsA(AProjectOYGCharacter::StaticClass()))
	{
		InflictDamage();
	}
}

void UProjectOYGEnemyHitBox::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	//UE_LOG(LogTemp, Warning, TEXT("overlap end"));
	if(OtherActor->IsA(AProjectOYGCharacter::StaticClass()))
	{
		IsOverlapping = false;
	}
}

void UProjectOYGEnemyHitBox::ActivateHitBox(float Damage) 
{
	Active = true;
	HitBoxDamage = Damage;

	//GetWorldTimerManager().SetTimer(DeactivationHandle, eventTest, &UProjectOYGEnemyHitBox::DeactivateHitBox, Time, false);
}

void UProjectOYGEnemyHitBox::DeactivateHitBox() 
{
	Active = false;
	HasHit = false;
}

void UProjectOYGEnemyHitBox::SetPoiseDmg(float PoiseDamage) 
{
	PoiseDmg = PoiseDamage;
}

void UProjectOYGEnemyHitBox::InflictDamage() 
{
	if(ActorToDamage == nullptr){return;}
	HasHit = true; 

	if((ActorToDamage->IsA(AProjectOYGCharacter::StaticClass())))
	{
		// Used for poise calculation - determine whether character is staggered
		Cast<AProjectOYGCharacter>(ActorToDamage)->SetPoiseDmgTaken(PoiseDmg);
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr){return;}
	AController* OwnerController = OwnerPawn->GetController();
	if(OwnerController == nullptr){return;}

	FPointDamageEvent DamageEvent; // nothing for now, can add for different hit effects possibly

	ActorToDamage->TakeDamage(HitBoxDamage, DamageEvent, OwnerController, nullptr); // could just set active to false here
}


