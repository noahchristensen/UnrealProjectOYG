// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGGameMode_Default.h"


void AProjectOYGGameMode_Default::PawnKilled(APawn* PawnKilled) 
{
    Super::PawnKilled(PawnKilled);

    APlayerController* PlayerController = Cast<APlayerController>(PawnKilled);
    if(PlayerController != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerController Success"));
        PlayerController->GameHasEnded(nullptr, false);
    }
}
