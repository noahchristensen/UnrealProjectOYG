// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectOYGSavePoint.generated.h"

UCLASS()
class PROJECTOYG_API AProjectOYGSavePoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectOYGSavePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Show(bool IsVisible);
	void Use();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPointLightComponent* PointLight;

	void Activate();
	void Rest();

private:
	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* MainCollision;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		UParticleSystem *ImpactEffect;

	bool Activated;
};
