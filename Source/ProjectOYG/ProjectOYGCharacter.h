// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TriggerVolume.h"
#include "ProjectOYGWeapon.h"
#include "Components/AudioComponent.h"
#include "ProjectOYGCharacter.generated.h"

class AProjectOYGArmor;
class AProjectOYGWeapon;
class AProjectOYGItem;
class AProjectOYGEnemy_NPC;

UCLASS(config=Game)
//UCLASS(transient, Blueprintable, hideCategories=AnimInstance, BlueprintType)
class AProjectOYGCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	
public:
	AProjectOYGCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, Category=Camera)
	float BaseLookUpRate;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	virtual void AddControllerYawInput(float Val) override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UAnimMontage* AttackMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* ArmorHeadMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* ArmorChestMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* ArmorHandsMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* ArmorLegsMesh = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* ArmorFeetMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* PotionMesh = nullptr;

	virtual void BeginPlay() override;

	virtual void Tick( float DeltaTime ) override;

	virtual void Jump() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;

	// Functions Accessible in Blueprint
	UFUNCTION(BlueprintPure)
		bool GetIsDead() const;
	UFUNCTION(BlueprintPure)
		bool GetIsStaggered() const;
	UFUNCTION(BlueprintPure)
		float GetHealthPercent() const;
	UFUNCTION(BlueprintPure)
		float GetStaminaPercent() const;
	UFUNCTION(BlueprintPure)
		AProjectOYGWeapon*  GetWeapon() const;
	UFUNCTION(BlueprintPure)
		int GetIsAttacking() const;
	UFUNCTION(BlueprintPure)
		bool GetIsBackStabbing() const;
	UFUNCTION(BlueprintPure)
		bool GetIsComboing() const;
	UFUNCTION(BlueprintPure)
		bool GetIsPickingUp() const;
	UFUNCTION(BlueprintPure)
		bool GetIsSheathing() const;
	UFUNCTION(BlueprintPure)
		bool GetIsUnSheathing() const;
	UFUNCTION(BlueprintPure)
		bool GetIsClimbingLedge() const;
	UFUNCTION(BlueprintPure)
		bool GetIsDrinking() const;
	UFUNCTION(BlueprintPure)
		int GetHealsLeft() const;
	UFUNCTION(BlueprintPure)
		bool GetIsLockedOn() const;
	UFUNCTION(BlueprintPure)
		bool GetIsOverlapping() const;
	UFUNCTION(BlueprintPure)
		bool GetIsOverlappingTwo() const;
	UFUNCTION(BlueprintPure)
		FString GetOverlapMessage() const;
	UFUNCTION(BlueprintPure)
		FString GetOverlapMessageSecondary() const;
	UFUNCTION(BlueprintPure)
		FString GetSubtitles() const;


	void AddToInventory(AProjectOYGItem* Item);

	UFUNCTION(BlueprintCallable)
		void Equip(AProjectOYGItem* Item);
	UFUNCTION(BlueprintCallable)
		void UnEquip(AProjectOYGItem* Item);
	
	void SetPoiseDmgTaken(float PoiseDmg);
	
private:

	/* MOVEMENT */
	void MoveForward(float Value);
	void MoveRight(float Value);
	/* CAMERA */
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	/* ROLL AND SPRINT */
	void WaitForRoll();
	void Sprint();
	void StopSprinting();
	void CheckCancelSprint(float DeltaTime);
	void Roll();
	void StopRolling();
	void ResetRoll();
	/* CLIMB */
	void StartClimb();
	void Climb();
	void ClimbLedge();
	void StopClimb();
	void StopClimbLedge();
	/* LOCKON */
	void LockOn();
	void SwitchLockRight();
	void SwitchLockLeft();
	void StopLockOn();
	void SwitchCooldownOver();
	void UpdateLockCamera(float DeltaTime);
	void CheckLocked();
	bool ValidForLockOn(float Distance, class AProjectOYGEnemy* CurrentEnemy);
	void AdjustSight();
	/* STAMINA */
	bool CanStaminaRegen();
	void StaminaRegen(float DeltaTime);
	void WaitForStaminaCooldown();
	/* ATTACK */
	void PoiseRegen(float DeltaTime);
	void LightAttack();
	void LightAttackEnd();
	void LightAttackCombo();
	void StopLightAttack();
	void BackStab();
	void BackStabHit();
	void HeavyAttack();
	void FallingAttack();
	void FallingAttackLand();
	void StopAttack();
	void StopStagger();
	float GetDamageReductionTotal();
	float GetPoiseTotal();
	/* INVENTORY*/
	void StartPickUp();
	void PickUp();
	void StopPickUp();
	// UFUNCTION(BlueprintCallable)
	// void Equip(AProjectOYGItem* Item); // In public to be called by playercontroller
	// UFUNCTION(BlueprintCallable)
	// void UnEquip(AProjectOYGItem* Item);
	/* MISC ACTION */
	void ToggleSheathRight();
	void SheathRight();
	void SheathLeft();
	void UnSheathRight();
	void UnSheathLeft();
	void StartSheath();
	void StartUnSheath();
	void StopSheath();
	void StopUnSheath();
	void StartDrink();
	void Drink();
	void StopDrink();
	void DrinkRegen(float DeltaTime);
	void StopDrinkRegen();
	void StartRest();
	void StopRest();
	void Talk();
	/* HELPERS, ETC */
	void StopAllActions();
	bool PerformingAction();

	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Editable Variables
	/* STATS */
	UPROPERTY(EditAnywhere, Category = "Stats")
		USkeletalMesh* DefaultMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = "Stats")
		USkeletalMesh* NakedMesh;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float Health;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float Stamina;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float MaxHealth;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float MaxStamina;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float StaminaRegenAmount;
	UPROPERTY(EditAnywhere, Category = "Stats")
		float StaminaCooldown;
	UPROPERTY(EditAnywhere, Category = "Stats")
		int Heals;
	/* MOVEMENT */
	UPROPERTY(EditAnywhere, Category = "Movement")
		float SprintSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float SprintStaminaCost;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float JogSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float RollSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float RollCooldown;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float RollStop;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float RollDeceleration;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float RollStaminaCost;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float SprintStartUp; // also time to determine roll
	UPROPERTY(EditAnywhere, Category = "Movement")
		float ClimbStartTime;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float ClimbLedgeTime;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float ClimbSpeed;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float SprintJumpHeight;
	/* LOCK ON */
	UPROPERTY(EditAnywhere, Category = "Lockon")
		class AActor* SpecificActor;
	UPROPERTY(EditAnywhere, Category = "Lockon")
		float MinLockDist;
	UPROPERTY(EditAnywhere, Category = "Lockon")
		float LockOnInterp;
	UPROPERTY(EditAnywhere, Category = "Lockon")
		float LockOnPeripheral;
	UPROPERTY(EditAnywhere, Category = "Lockon")
		float LockOnSpeed;
	UPROPERTY(EditAnywhere, Category = "Lockon")
		float SwitchTargetCoolDown;
	/* COMBAT */
	UPROPERTY(EditAnywhere, Category = "Combat")
		float CurrentPoise;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float PoiseRegenAmount;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float StaggerDuration;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float DmgReductionTotal;
	UPROPERTY(EditAnywhere, Category = "Combat")
		float FallingAtkEndLag;
	/* Equipment */
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AProjectOYGWeapon> UnarmedWeaponClass;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AProjectOYGWeapon> WeaponClass;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		AProjectOYGWeapon* Weapon;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AProjectOYGArmor> ArmorHeadClass;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		AProjectOYGArmor* ArmorHead;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AProjectOYGArmor> ArmorChestClass;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		AProjectOYGArmor* ArmorChest;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AProjectOYGArmor> ArmorHandsClass;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		AProjectOYGArmor* ArmorHands;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AProjectOYGArmor> ArmorLegsClass;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		AProjectOYGArmor* ArmorLegs;
	UPROPERTY(EditDefaultsOnly, Category = "Equipment")
		TSubclassOf<AProjectOYGArmor> ArmorFeetClass;
	UPROPERTY(EditAnywhere, Category = "Equipment")
		AProjectOYGArmor* ArmorFeet;
	/* MISC */
	UPROPERTY(EditAnywhere, Category = "Misc")
		float SheathStartTime;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float SheathEndTime;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float StartPickUpTime;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float EndPickUpTime;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float DrinkStartTime;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float DrinkEndTime;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float DrinkRegenAmount;
	UPROPERTY(EditAnywhere, Category = "Misc")
		float DrinkRegenTime;
	/* AUDIO */
	UPROPERTY(EditAnywhere, Category = "Sound")
		class USoundCue* PickUpSoundCue;

	// Private Variables
	bool InRollCooldown; // not on cooldown, not in air
	bool CanRoll;
	bool CanClimb; // next to valid object
	bool OnLedge;
	bool IsStartClimbing;
	bool IsClimbing;
	bool IsClimbingLedge;
	bool IsSprinting;
	bool IsLockedOn;
	bool IsRolling;
	bool InStaminaCooldown;
	bool StaminaActionTriggered;
	bool IsInvulnerable;
	bool IsStaggered;
	bool DeadScreenActive;
	bool AttackQueued;
	bool RightSheathed;
	bool IsPickingUp;
	bool IsSheathing;
	bool IsUnSheathing;
	bool IsDrinking;
	bool InDrinkRegen;
	bool CanPickUp;
	bool CanRest;
	bool CanTalk;
	bool IsResting; // used tent
	bool IsRested; // has rested / has already respawned enemies and refreshed healing
	bool IsLanding;
	bool IsOverlapping;
	bool IsBackStabbing;
	bool BackStabOverlap;
	bool CanSwitchTarget; // can switch target for use in lock on
	bool InComboEndBuffer; // buffer for combos if player chooses to attack 'late'
	bool ChoseCombo;
	int IsAttacking;
	FTimerHandle UnusedHandle;
	FTimerHandle UnusedHandleTwo;
	FTimerHandle UnusedHandleThree;
	FTimerHandle UnusedHandleFour;
	FTimerHandle UnusedHandleFive;
	FTimerHandle UnusedHandleSix;
	FTimerHandle UnusedHandleSeven;
	FTimerHandle UnusedHandleEight;
	FTimerHandle UnusedHandleNine;
	FTimerHandle SwitchTimer;
	float InitialBrakingFactor;
	float InitialGroundFriction;
	float InitialGroundDeceleration;
	float InitialAirDeceleration;
	float InitialJumpHeight;
	class AActor* ClimbActor;
	class AProjectOYGItem* ItemActor;
	class AProjectOYGEnemy_NPC* NPCActor;
	class AProjectOYGSavePoint* SaveActor;
	class AProjectOYGEnemy* NearestEnemy;
	float NearestEnemyDist;
	class AProjectOYGEnemy* CurrentLockedEnemy;
	float CurrentLockedEnemyDist;

	FVector SpawnLocation;

	class AProjectOYGEnemy_Humanoid* BackStabEnemy;

	float PoiseDmgTaken; // SetByEnemy when attacked

	UAudioComponent* PickUpAudioComponent;
};
