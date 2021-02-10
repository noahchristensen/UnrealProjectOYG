// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectOYGHitBox.generated.h"

UCLASS()
class PROJECTOYG_API AProjectOYGHitBox : public AActor
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Collider;

public:	
	// Sets default values for this actor's properties
	AProjectOYGHitBox();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ActivateHitBox(float Time, float Damage);
	void DeactivateHitBox();

private:

	bool Active;
	bool HasHit;
	float HitBoxDamage;
	FTimerHandle DeactivationHandle;
};
