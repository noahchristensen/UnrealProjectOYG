// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGItem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "ProjectOYGCharacter.h"

// Sets default values
AProjectOYGItem::AProjectOYGItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	MainCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MainCollision"));
	MainCollision->SetupAttachment(Mesh);

	Name = "";
	IsEquipped = false;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	PointLight->SetupAttachment(Mesh);
	PointLight->Intensity = 300.f;
	PointLight->SetVisibility(true);
}

// Called when the game starts or when spawned
void AProjectOYGItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectOYGItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FString AProjectOYGItem::GetName() 
{
	return Name;
}

void AProjectOYGItem::Show(bool IsVisible) 
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

void AProjectOYGItem::ItemGrab() 
{
	UE_LOG(LogTemp, Warning, TEXT("grab"));

	AProjectOYGCharacter* Player = Cast<AProjectOYGCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if(Player != nullptr)
	{
		Show(false);

		Player->AddToInventory(this);
	}
}

bool AProjectOYGItem::GetIsEquipped() 
{
	return IsEquipped;
}

void AProjectOYGItem::SetIsEquipped(bool Equip) 
{
	IsEquipped = Equip;
}
