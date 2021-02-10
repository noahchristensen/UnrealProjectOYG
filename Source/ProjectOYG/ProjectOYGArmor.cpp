// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGArmor.h"
#include "Components/SkeletalMeshComponent.h"

AProjectOYGArmor::AProjectOYGArmor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	SlotID = 0;
	DmgReduction = 0;
	Poise = 0;
}

void AProjectOYGArmor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectOYGArmor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int AProjectOYGArmor::GetSlotID() 
{
	return SlotID;
}

float AProjectOYGArmor::GetDmgReduction() 
{
	return DmgReduction;
}

float AProjectOYGArmor::GetPoise() 
{
	return Poise;
}


USkeletalMeshComponent* AProjectOYGArmor::GetMesh() 
{
	return Mesh;
}

