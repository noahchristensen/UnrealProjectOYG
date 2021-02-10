// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGEnemy_NPC.h"
#include "ProjectOYGCharacter.h"


AProjectOYGEnemy_NPC::AProjectOYGEnemy_NPC() 
{
    Collider = CreateDefaultSubobject<USphereComponent>(TEXT("DialogueCollider"));
    Collider->SetupAttachment(GetMesh(), TEXT("HeadSocket"));
	//Collider->RegisterComponent();
    Collider->SetCollisionObjectType(ECC_GameTraceChannel4);
}

void AProjectOYGEnemy_NPC::BeginPlay() 
{
    Super::BeginPlay();

    Collider->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("HeadSocket"));
}

void AProjectOYGEnemy_NPC::PlayDialogue() 
{
    if(Dialogue.Num() != DialogueTime.Num())
    {
        UE_LOG(LogTemp, Error, TEXT("Dialogue and Dialogue Time amount do not match"));
        return;
    }

    PlayDialogueIndex(0);
}

void AProjectOYGEnemy_NPC::PlayDialogueIndex(int i) 
{
    if(Dialogue.Num() > i)
    {
        Subtitles = Dialogue[i];
        UE_LOG(LogTemp, Warning, TEXT("Dialogue"));

        FTimerDelegate NextDelegate = FTimerDelegate::CreateUObject( this, &AProjectOYGEnemy_NPC::PlayDialogueIndex, (i + 1));
        GetWorldTimerManager().SetTimer(DialogueHandle, NextDelegate, DialogueTime[i], false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Dialogue fail"));
        Subtitles = "";
    }
}

FString AProjectOYGEnemy_NPC::GetSubtitles() 
{
    return Subtitles;
}


