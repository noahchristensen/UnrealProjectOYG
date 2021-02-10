// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectOYGGameMode.generated.h"

UCLASS(minimalapi)
class AProjectOYGGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectOYGGameMode();

	virtual void PawnKilled(APawn* PawnKilled);
};



