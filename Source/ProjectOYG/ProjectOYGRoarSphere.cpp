// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGRoarSphere.h"
#include "ProjectOYGEnemy.h" 
#include "ProjectOYGCharacter.h" 
#include "Components/SphereComponent.h"

// Sets default values for this component's properties
UProjectOYGRoarSphere::UProjectOYGRoarSphere()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	//Collider->RegisterComponent();
	Collider->SetupAttachment(this);
	//Collider->SetMobility(EComponentMobility::Movable);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &UProjectOYGRoarSphere::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &UProjectOYGRoarSphere::OnOverlapEnd);

	TickDamage = 0.f;
	IsOverlapping = false;
}


// Called when the game starts
void UProjectOYGRoarSphere::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UProjectOYGRoarSphere::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(ActorToDamage == nullptr){return;}
	if(IsOverlapping && Active && ActorToDamage->IsA(AProjectOYGCharacter::StaticClass()))
	{
		InflictDamage();
	}
}

void UProjectOYGRoarSphere::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if(OtherActor->IsA(AProjectOYGCharacter::StaticClass()))
	{
		ActorToDamage = OtherActor;
		IsOverlapping = true;
	}
}

void UProjectOYGRoarSphere::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) 
{
	if(OtherActor->IsA(AProjectOYGCharacter::StaticClass()))
	{
		IsOverlapping = false;
	}
}

void UProjectOYGRoarSphere::ActivateHitBox(float Damage) 
{
	TickDamage = Damage;
	Active = true;
}

void UProjectOYGRoarSphere::DeactivateHitBox() 
{
	Active = false;
}

void UProjectOYGRoarSphere::InflictDamage() 
{
	if(ActorToDamage == nullptr){return;}

	if((ActorToDamage->IsA(AProjectOYGCharacter::StaticClass())))
	{
		// Used for poise calculation - determine whether character is staggered
		Cast<AProjectOYGCharacter>(ActorToDamage)->SetPoiseDmgTaken(-1.f);
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if(OwnerPawn == nullptr){return;}
	AController* OwnerController = OwnerPawn->GetController();

	FPointDamageEvent DamageEvent; // nothing for now, can add for different hit effects possibly

	ActorToDamage->TakeDamage(TickDamage, DamageEvent, OwnerController, nullptr);
}
