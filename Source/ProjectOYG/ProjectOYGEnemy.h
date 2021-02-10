// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectOYGEnemy.generated.h"

class AProjectOYGWeapon;

UCLASS()
class PROJECTOYG_API AProjectOYGEnemy : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LockOnVisual;

public:
	// Sets default values for this character's properties
	AProjectOYGEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	FORCEINLINE class UStaticMeshComponent* GetLockOnVisual() const { return LockOnVisual; }
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;

	void TakeDamageLaunch(FVector LaunchDirection, float LaunchDistance);

	UFUNCTION(BlueprintPure)
		bool GetIsDead() const;
	UFUNCTION(BlueprintPure)
		bool GetIsStaggered() const;
	UFUNCTION(BlueprintPure)
		float GetHealthPercent() const;
	UFUNCTION(BlueprintPure)
		int GetIsAttacking() const;

	float DistanceFromPlayer();
	void TargetLockOn();
	void TargetLockOff();

	virtual void StopAttack();
	virtual void StopAllActions();
	void StopStagger();
	virtual void Die();
	bool PerformingAction();

	void SetIsStaggered(bool Stagger);
	void SetIsAttacking(int Attack);
	void SetHealth(float NewHealth);
	float GetDefaultWalkSpeed();
	float GetDefaultTurnSpeed();
	float GetHealth();

	void Bleed(FVector Location, FRotator Rotation);

	void SetPoiseDmgTaken(float PoiseDmg);

	FVector GetInitialLocation();
	FRotator GetInitialRotation();

	UPROPERTY(EditAnywhere)
		bool ShouldRespawn;

	UPROPERTY(EditAnywhere)
		UParticleSystem *ImpactEffect;

private:

	void PoiseRegen();

	/* STATS */
	UPROPERTY(EditAnywhere, Category = "Stats")
		float Health;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float MaxHealth;
	/* COMBAT */
	UPROPERTY(EditAnywhere, Category = "Combat")
		float StaggerDuration;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float MaxPoise;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float PoiseRegenAmount;

	FVector InitialLocation;
	FRotator InitialRotation;

	float DefaultWalkSpeed;
	float DefaultTurnSpeed;
	bool IsStaggered;
	int IsAttacking;
	float CurrentPoise;
	float PoiseDmgTaken; // SetByEnemy when attacked
	//FTimerHandle UnusedHandle;
	FTimerHandle UnusedHandleTwo;

};
