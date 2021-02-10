// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectOYGArmor.generated.h"

UCLASS()
class PROJECTOYG_API AProjectOYGArmor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AProjectOYGArmor();

protected:
	
	virtual void BeginPlay() override;

public:	
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		int GetSlotID();
	UFUNCTION(BlueprintCallable)
		float GetDmgReduction();
	UFUNCTION(BlueprintCallable)
		float GetPoise();
	UFUNCTION(BlueprintCallable)
		USkeletalMeshComponent*  GetMesh();

private:
	
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
		USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		int SlotID; // 1 - Head, 2 - Chest, 3 - Hands, 4 - Legs, 5 - Feet, 0 - none
	UPROPERTY(EditAnywhere)
		float DmgReduction; // 1 - Head, 2 - Chest, 3 - Hands, 4 - Legs, 5 - Feet, 0 - none
	UPROPERTY(EditAnywhere)
		float Poise; // 1 - Head, 2 - Chest, 3 - Hands, 4 - Legs, 5 - Feet, 0 - none
};
