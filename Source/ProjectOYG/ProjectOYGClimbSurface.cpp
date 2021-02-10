// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGClimbSurface.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AProjectOYGClimbSurface::AProjectOYGClimbSurface()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	MainCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MainCollision"));
	MainCollision->SetupAttachment(Mesh);

	LedgeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LedgeCollision"));
	LedgeCollision->SetupAttachment(MainCollision);
	LedgeCollision->SetCollisionObjectType(ECC_GameTraceChannel3);
}

// Called when the game starts or when spawned
void AProjectOYGClimbSurface::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AProjectOYGClimbSurface::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

FVector AProjectOYGClimbSurface::GetTopLocation() 
{
	return FVector(LedgeCollision->GetComponentLocation().X, LedgeCollision->GetComponentLocation().Y, LedgeCollision->GetComponentLocation().Z + 20.f);
}

