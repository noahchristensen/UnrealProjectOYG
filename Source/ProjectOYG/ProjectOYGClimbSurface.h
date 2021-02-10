// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectOYGClimbSurface.generated.h"

UCLASS()
class PROJECTOYG_API AProjectOYGClimbSurface : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectOYGClimbSurface();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetTopLocation();

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* MainCollision;
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* LedgeCollision;
};
