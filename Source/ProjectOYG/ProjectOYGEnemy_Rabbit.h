// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGEnemyHitBox.h"
#include "ProjectOYGEnemy_Rabbit.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGEnemy_Rabbit : public AProjectOYGEnemy
{
	GENERATED_BODY()
	
	//class AProjectOYGHitBox;
	//class UProjectOYGEnemyHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGEnemyHitBox* FeetHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGEnemyHitBox* HornHitBox;

	public:
		AProjectOYGEnemy_Rabbit();

	protected:
		virtual void BeginPlay() override;

	public:
		virtual void Tick(float DeltaTime) override;

		virtual void StopAttack();
		virtual void StopAllActions();

		/* 2 attacks, 
			Jump -> IsAttacking = 1, 
			FrontKick -> IsAttacking = 2
		*/
		void JumpStart();
		void JumpAttack();
		void KickFrontStart();
		void KickFrontAttack();

		
	private:
		UPROPERTY(EditAnywhere, Category = "Kick Attack")
			float KickAttackDamage;
		UPROPERTY(EditAnywhere, Category = "Kick Attack")
			float KickStartUp;
		UPROPERTY(EditAnywhere, Category = "Kick Attack")
			float KickHitDuration;
		UPROPERTY(EditAnywhere, Category = "Kick Attack")
			float KickPoiseDmg;
		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpAttackDamage;
		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpStartUp;
		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpHitDuration;
		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpMagnitude;
		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpPoiseDmg;

		FTimerHandle DeactivationHandle;
};
