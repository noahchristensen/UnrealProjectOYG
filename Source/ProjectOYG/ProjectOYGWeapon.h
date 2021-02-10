// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Animation/AnimSequence.h"
#include "Components/AudioComponent.h"
#include "ProjectOYGWeapon.generated.h"

class AProjectOYGEnemy;

UCLASS()
class PROJECTOYG_API AProjectOYGWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectOYGWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Helper Methods
	float GetLightAttackStaminaCost();
	float GetHeavyAttackStaminaCost();
	float GetLightAttackDuration();
	float GetComboEndBufferDuration();
	float GetHeavyAttackDuration();

	float GetBackStabDuration();
	float GetBackStabDistance();
	float GetBackStabHitTime();
	float GetBackStabDamage();

	void StartAttack(int AtkID, bool Combo);
	void ActivateHitbox();
	void DeactivateHitbox();
	UFUNCTION(BlueprintCallable)
		int GetWeaponTypeID();
	UFUNCTION(BlueprintCallable)
		bool GetCombos();
	UFUNCTION(BlueprintCallable)
		float GetPoiseDmg();
	UFUNCTION(BlueprintCallable)
		bool GetIsComboSecondHit();
	
	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* MainCollision;

	//UFUNCTION()
	//void OnCompHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	void Attacking();

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float LightAttackStaminaCost;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float LightAttackStartup;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float LightAttackHitDuration;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float LightAttackDuration;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float ComboLightAttackStartup;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float ComboLightAttackHitDuration;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float ComboLightAttackDuration;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float ComboEndBufferDuration;
	UPROPERTY(EditAnywhere, Category = "Heavy Attack")
		float HeavyAttackStaminaCost;
	UPROPERTY(EditAnywhere, Category = "Heavy Attack")
		float HeavyAttackStartup;
	UPROPERTY(EditAnywhere, Category = "Heavy Attack")
		float HeavyAttackHitDuration;
	UPROPERTY(EditAnywhere, Category = "Heavy Attack")
		float HeavyAttackDuration;
	UPROPERTY(EditAnywhere, Category = "Fall Attack")
		float FallingAttackStaminaCost;
	UPROPERTY(EditAnywhere, Category = "Fall Attack")
		float FallingAttackStartup;
	UPROPERTY(EditAnywhere, Category = "Fall Attack")
		float FallingAttackEndlag;
	UPROPERTY(EditAnywhere, Category = "Light Attack")
		float LightLaunchDistance;
	UPROPERTY(EditAnywhere, Category = "Heavy Attack")
		float HeavyLaunchDistance;
	UPROPERTY(EditAnywhere, Category = "Fall Attack")
		float FallLaunchDistance;
	UPROPERTY(EditAnywhere, Category = "Back Stab")
		float BackStabDuration;
	UPROPERTY(EditAnywhere, Category = "Back Stab")
		float BackStabHitTime;
	UPROPERTY(EditAnywhere, Category = "Back Stab")
		float BackStabDistance;
	
	UPROPERTY(EditAnywhere, Category = "Stats")
		float LightAtkDamage;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float HeavyAtkDamage;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float FallingAtkDamage;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float BackStabDamage;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float WeaponLength;
	UPROPERTY(EditAnywhere, Category = "Stats")
		bool Combos;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float PoiseDmg;
	UPROPERTY(EditAnywhere, Category = "Stats")
		int TypeID;

	UPROPERTY(EditAnywhere, Category = "Sounds")
		class USoundCue* AttackSoundCue;
	UPROPERTY(EditAnywhere, Category = "Sounds")
		class USoundCue* HitSoundCue;

	TArray<AActor*> EnemyHitArr;
	bool IsCombo;
	bool HitBoxActive;
	bool IsFallingAtk;
	float Damage;
	float LaunchDistance;
	int SavedAtkID;
	int ComboNum;
	FTimerHandle UnusedHandle;

	UAudioComponent* AttackAudioComponent;
	UAudioComponent* HitAudioComponent;
};
