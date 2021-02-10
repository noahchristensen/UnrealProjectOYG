// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "ProjectOYGEnemyHitBox.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTOYG_API UProjectOYGEnemyHitBox : public USceneComponent
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* Collider;

public:	
	// Sets default values for this component's properties
	UProjectOYGEnemyHitBox();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ActivateHitBox(float Damage);
	void DeactivateHitBox();
	void SetPoiseDmg(float PoiseDamage);
	
private:
	void InflictDamage();

	AActor* ActorToDamage;

	bool Active;
	float PoiseDmg;
	bool HasHit;
	bool IsOverlapping; // used if hit is activated when ALREADY overlapping
	float HitBoxDamage;
	FTimerHandle DeactivationHandle;
};
