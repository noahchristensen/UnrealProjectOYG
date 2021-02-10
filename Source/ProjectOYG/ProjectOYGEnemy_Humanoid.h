// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGEnemy_Humanoid.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGEnemy_Humanoid : public AProjectOYGEnemy
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BackStabCollider;

public:
	AProjectOYGEnemy_Humanoid();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

public:
	UFUNCTION(BlueprintPure)
		bool GetIsBackStabbed() const;

	bool IsBackStabable();
	void BackStabFreeze(float BackStabTime);
	void BackStabUnFreeze();
	float BackStabTakeDamage(float DamageAmount);

	void LightAttack();
	void HeavyAttack();

private:

	virtual void StopAttack() override;
	virtual void StopAllActions() override;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<AProjectOYGWeapon> WeaponClass;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		AProjectOYGWeapon* Weapon;

	bool IsBackStabbed;
	FTimerHandle UnusedHandle;
};
