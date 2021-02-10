// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGGameMode.h"
#include "ProjectOYGGameMode_Default.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGGameMode_Default : public AProjectOYGGameMode
{
	GENERATED_BODY()
	
	public:

		virtual void PawnKilled(APawn* PawnKilled) override;
};
