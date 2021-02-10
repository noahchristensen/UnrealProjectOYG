// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGProjectile.h"
#include "ProjectOYGEnemy_Slug.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGEnemy_Slug : public AProjectOYGEnemy
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
	class UProjectOYGEnemyHitBox* HeadHitBox;

	public:
		AProjectOYGEnemy_Slug();

	protected:
		virtual void BeginPlay() override;

		UPROPERTY(EditDefaultsOnly, Category = Projectile)
			TSubclassOf<class AProjectOYGProjectile> ProjectileClass;

	public:
		virtual void Tick(float DeltaTime) override;

		virtual void StopAttack();
		virtual void StopAllActions();

		void StrikeStart();
		void StrikeAttack();
		void SpitStart();
		void SpitAttack();

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
			FVector MuzzleOffset;
		
	private:

		UPROPERTY(EditAnywhere, Category = "Strike Attack")
			float StrikeAttackDamage;
		UPROPERTY(EditAnywhere, Category = "Strike Attack")
			float StrikeStartUp;
		UPROPERTY(EditAnywhere, Category = "Strike Attack")
			float StrikeHitDuration;
		UPROPERTY(EditAnywhere, Category = "Strike Attack")
			float StrikeLaunchMagnitude;
		UPROPERTY(EditAnywhere, Category = "Strike Attack")
			float StrikePoiseDmg;
		UPROPERTY(EditAnywhere, Category = "Spit Attack")
			float SpitStartUp;
		UPROPERTY(EditAnywhere, Category = "Spit Attack")
			float SpitEndLag;
		UPROPERTY(EditAnywhere, Category = "Spit Attack")
			float SpitPoiseDmg;

		FTimerHandle DeactivationHandle;
};
