// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectOYGItem.generated.h"

UCLASS()
class PROJECTOYG_API AProjectOYGItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectOYGItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintPure)
	virtual FString GetName();

	virtual void Show(bool Visible);
	void ItemGrab();

	UFUNCTION(BlueprintPure)
	bool GetIsEquipped();
	void SetIsEquipped(bool Equip);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Image;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPointLightComponent* PointLight;

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* MainCollision;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		FString Name;

	bool IsEquipped;
	
};
