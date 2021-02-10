// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGEnemyHitBox.h"
#include "ProjectOYGRoarSphere.h"
#include "ProjectOYGEnemy_Bear.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGEnemy_Bear : public AProjectOYGEnemy
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGEnemyHitBox* LeftPawHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGEnemyHitBox* RightPawHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGEnemyHitBox* HeadHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGEnemyHitBox* DiveHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGEnemyHitBox* FullBodyHitBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = StaticMesh, meta = (AllowPrivateAccess = "true"))
		class UProjectOYGRoarSphere* RoarHitSphere;

	public:
		AProjectOYGEnemy_Bear();

	protected:
		virtual void BeginPlay() override;

	public:
		virtual void Tick(float DeltaTime) override;

		virtual void StopAttack();
		virtual void StopAllActions();

		/* 2 attacks, 
			OneSwipe -> IsAttacking = 1, 
			TwoSwipes -> IsAttacking = 2,
			Head -> IsAttacking = 3, 
			Dive -> IsAttacking = 4, 
			Jump -> IsAttacking = 5,
			Roar -> IsAttacking = 6,  
		*/

		void Die() override;

		void OneSwipeStart();
		void OneSwipeAttack();
		void OneSwipeEnd();
		void TwoSwipeStart();
		void TwoSwipeAttack();
		void TwoSwipeTransition();
		void TwoSwipeAttackTwo();
		void TwoSwipeEnd();
		void HeadStart();
		void HeadAttack();
		void HeadEnd();
		void DiveStart();
		void DiveAttack();
		void DiveEnd();
		void JumpStart();
		void JumpAction();
		void JumpEnd();
		void RoarStart();
		void RoarAttack();
		void RoarEnd();

	private:
		UPROPERTY(EditAnywhere, Category = "Movement")
			float RunSpeed;
		UPROPERTY(EditAnywhere, Category = "Movement")
			float WalkSpeed;
		UPROPERTY(EditAnywhere, Category = "One-Swipe Attack")
			float SwipeDamage;
		UPROPERTY(EditAnywhere, Category = "One-Swipe Attack")
			float SwipePoiseDamage;
		UPROPERTY(EditAnywhere, Category = "One-Swipe Attack")
			float OneSwipeStartDuration;
		UPROPERTY(EditAnywhere, Category = "One-Swipe Attack")
			float OneSwipeAttackDuration;
		UPROPERTY(EditAnywhere, Category = "One-Swipe Attack")
			float OneSwipeEndDuration;

		UPROPERTY(EditAnywhere, Category = "Two-Swipe Attack")
			float TwoSwipeStartDuration;
		UPROPERTY(EditAnywhere, Category = "Two-Swipe Attack")
			float TwoSwipeAttackDuration;
		UPROPERTY(EditAnywhere, Category = "Two-Swipe Attack")
			float TwoSwipeTranistionDuration;
		UPROPERTY(EditAnywhere, Category = "Two-Swipe Attack")
			float TwoSwipeAtttackTwoDuration;
		UPROPERTY(EditAnywhere, Category = "Two-Swipe Attack")
			float TwoSwipeEndDuration;

		UPROPERTY(EditAnywhere, Category = "Head Attack")
			float HeadDamage;
		UPROPERTY(EditAnywhere, Category = "Head Attack")
			float HeadPoiseDamage;
		UPROPERTY(EditAnywhere, Category = "Head Attack")
			float HeadStartDuration;
		UPROPERTY(EditAnywhere, Category = "Head Attack")
			float HeadAttackDuration;
		UPROPERTY(EditAnywhere, Category = "Head Attack")
			float HeadEndDuration;

		UPROPERTY(EditAnywhere, Category = "Dive Attack")
			float FullBodyPoiseDamage;

		UPROPERTY(EditAnywhere, Category = "Dive Attack")
			float DiveDamage;
		UPROPERTY(EditAnywhere, Category = "Dive Attack")
			float DivePoiseDamage;
		UPROPERTY(EditAnywhere, Category = "Dive Attack")
			float DiveLaunchMagnitude;
		UPROPERTY(EditAnywhere, Category = "Dive Attack")
			float DiveStartDuration;
		UPROPERTY(EditAnywhere, Category = "Dive Attack")
			float DiveAttackDuration;
		UPROPERTY(EditAnywhere, Category = "Dive Attack")
			float DiveEndDuration;

		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpDamage;
		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpStartDuration;
		UPROPERTY(EditAnywhere, Category = "Jump Attack")
			float JumpLandDuration;

		UPROPERTY(EditAnywhere, Category = "Roar Attack")
			float RoarTickDamage;
		UPROPERTY(EditAnywhere, Category = "Roar Attack")
			float RoarStartDuration;
		UPROPERTY(EditAnywhere, Category = "Roar Attack")
			float RoarAttackDuration;
		UPROPERTY(EditAnywhere, Category = "Roar Attack")
			float RoarEndDuration;

		//UPROPERTY(EditAnywhere)
		//	UCameraShake* RoarShake;
		//UPROPERTY(EditAnywhere)
		//	TSubclassOf<UCameraShake> RoarShakeClass;

		bool HasJumped;
		
		FTimerHandle DeactivationHandle;
};
