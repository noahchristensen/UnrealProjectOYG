// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectOYGGameMode.h"
#include "ProjectOYGCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectOYGGameMode::AProjectOYGGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("Blueprint'/Game/Blueprints/GoblinCharacter_BP.GoblinCharacter_BP_C'"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AProjectOYGGameMode::PawnKilled(APawn* PawnKilled) 
{
	
}
