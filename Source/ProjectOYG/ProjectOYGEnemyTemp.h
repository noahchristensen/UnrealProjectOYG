// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectOYGEnemyTemp.generated.h"

class AProjectOYGWeapon;

UCLASS()
class PROJECTOYG_API AProjectOYGEnemyTemp : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* LockOnVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BackStabCollider;

public:
	// Sets default values for this character's properties
	AProjectOYGEnemyTemp();

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
	UFUNCTION(BlueprintPure)
		bool GetIsBackStabbed() const;

	bool IsBackStabable();
	void BackStabFreeze(float BackStabTime);
	void BackStabUnFreeze();
	float BackStabTakeDamage(float DamageAmount);

	float DistanceFromPlayer();
	void TargetLockOn();
	void TargetLockOff();

	void LightAttack();
	void HeavyAttack();

private:
	/* ATTACK */
	void StopAttack();
	void StopStagger();
	/* HELPERS, ETC */
	void StopAllActions();
	bool PerformingAction();

	void Die();

	/* STATS */
	UPROPERTY(EditAnywhere)
		float Health;
	UPROPERTY(EditAnywhere)
		float MaxHealth;
	/* COMBAT */
	UPROPERTY(EditAnywhere)
		float Poise;
	UPROPERTY(EditAnywhere)
		float StaggerDuration;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AProjectOYGWeapon> WeaponClass;
	UPROPERTY(EditAnywhere)
		AProjectOYGWeapon* Weapon;

	float DefaultWalkSpeed;
	float DefaultTurnSpeed;
	bool IsStaggered;
	bool IsBackStabbed;
	int IsAttacking;
	FTimerHandle UnusedHandle;
	FTimerHandle UnusedHandleTwo;

};
