// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGSavePoint.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "ProjectOYGCharacter.h"

// Sets default values
AProjectOYGSavePoint::AProjectOYGSavePoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	MainCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MainCollision"));
	MainCollision->SetupAttachment(Mesh);

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	PointLight->SetupAttachment(Mesh);
	PointLight->Intensity = 300.f;
	PointLight->SetVisibility(false);

	Activated = false;
}

// Called when the game starts or when spawned
void AProjectOYGSavePoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectOYGSavePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectOYGSavePoint::Show(bool IsVisible) 
{
	Mesh->SetVisibility(IsVisible);
	PointLight->SetVisibility(IsVisible);
	if(IsVisible)
	{
		MainCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		MainCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	SetActorTickEnabled(IsVisible);
}

void AProjectOYGSavePoint::Activate() 
{
	PointLight->SetVisibility(true);

	Activated = true;
}

void AProjectOYGSavePoint::Rest() 
{
	if(ImpactEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, GetActorLocation(), GetActorRotation());
	}
}

