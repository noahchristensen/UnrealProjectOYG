// Copyright Epic Games, Inc. All Rights Reserved.

/* TODO
	* unequip replaced items
	* dynamic inventory size
	* Project Settings - Engine - Collision 

	* world building OMG

	* fixed unlock bug(roll into enemy)
*/







#include "ProjectOYGCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Public/TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGEnemy_Humanoid.h"
#include "ProjectOYGWeapon.h"
#include "ProjectOYGArmor.h"
#include "ProjectOYGItem.h"
#include "ProjectOYGSavePoint.h"
#include "ProjectOYGItem_Weapon.h"
#include "ProjectOYGItem_Armor.h"
#include "ProjectOYGEnemy_NPC.h"
#include "ProjectOYGClimbSurface.h"
#include "ProjectOYGGameMode.h"
#include "ProjectOYGPlayerController.h"
#include "Sound/SoundCue.h"
#include "EngineUtils.h"

AProjectOYGCharacter::AProjectOYGCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(38.f, 86.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	 
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
	ArmorHeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorHead"));
	ArmorHeadMesh->SetupAttachment(GetMesh());
	ArmorChestMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorChest"));
	ArmorChestMesh->SetupAttachment(GetMesh());
	ArmorHandsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorHands"));
	ArmorHandsMesh->SetupAttachment(GetMesh());
	ArmorLegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorLegs"));
	ArmorLegsMesh->SetupAttachment(GetMesh());
	ArmorFeetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorFeet"));
	ArmorFeetMesh->SetupAttachment(GetMesh());

	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Potion"));
	PotionMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));

	static ConstructorHelpers::FObjectFinder<USoundCue> PickUpSoundCueObject(TEXT("SoundCue'/Game/Audio/PickUpSoundCue.PickUpSoundCue'"));
	if(PickUpSoundCueObject.Succeeded())
	{
		PickUpSoundCue = PickUpSoundCueObject.Object;

		PickUpAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PickUpAudioComponent"));
		PickUpAudioComponent->SetupAttachment(RootComponent);
	}

	UPrimitiveComponent* Collider = GetCapsuleComponent();
	Collider->OnComponentBeginOverlap.AddDynamic(this, &AProjectOYGCharacter::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &AProjectOYGCharacter::OnOverlapEnd);

	MaxHealth = 100.f;
	MaxStamina = 100.f;
	Health = 100.f;
	Stamina = 100.f;
	StaminaRegenAmount = 0.5f;
	StaminaCooldown = 1.f;
	SprintSpeed = 700.0f;
	SprintStaminaCost = 0.2f;
	JogSpeed = 400.0f;
	RollSpeed = 400.f;
	RollCooldown = 2.f;
	RollStop = 0.4f;
	RollDeceleration = 200.f;
	RollStaminaCost = 30.f;
	SprintStartUp = 0.2f;
	ClimbStartTime = 1.0f;
	ClimbLedgeTime = 4.9f;
	ClimbSpeed = 1.0f;
	SprintJumpHeight = 500.f;
	MinLockDist = 600.f;
	LockOnInterp = 7.f;
	LockOnPeripheral = 100.f;
	SwitchTargetCoolDown = 0.1f;
	StartPickUpTime = 0.4f;
	EndPickUpTime = 0.3f;
	SheathStartTime = 0.6f;
	SheathEndTime = 0.6f;
	DrinkStartTime = 0.6f;
	DrinkEndTime = 0.4f;
	DrinkRegenAmount = 0.5f;
	DrinkRegenTime = 1.5f;
	PoiseDmgTaken = 0.f;
	CurrentPoise = 0.f;
	PoiseRegenAmount = 0.01;
	FallingAtkEndLag = 0.7f;
	Heals = 3;

	InitialBrakingFactor = GetCharacterMovement()->BrakingFrictionFactor;
	InitialGroundFriction = GetCharacterMovement()->GroundFriction;
	InitialGroundDeceleration = GetCharacterMovement()->BrakingDecelerationWalking;
	InitialAirDeceleration = GetCharacterMovement()->BrakingDecelerationFalling;
	InitialJumpHeight = GetCharacterMovement()->JumpZVelocity;

	InRollCooldown = false;
	CanRoll = true;
	CanClimb = false;
	OnLedge = false;
	IsSprinting = false;
	IsStartClimbing = false;
	IsClimbing = false;
	IsClimbingLedge = false;
	IsRolling = false;
	IsInvulnerable = false;
	IsLockedOn = false;
	InStaminaCooldown = false;
	StaminaActionTriggered = false;
	IsAttacking = 0;
	ClimbActor = nullptr;
	IsStaggered = false;
	DeadScreenActive = false;
	AttackQueued = false;
	RightSheathed = false;
	IsPickingUp = false;
	IsSheathing = false;
	IsUnSheathing = false;
	IsDrinking = false;
	InDrinkRegen = false;
	CanPickUp = false;
	CanTalk = false;
	IsLanding = false;
	IsOverlapping = false;
	IsBackStabbing = false;
	BackStabOverlap = false;
	IsResting = false;
	IsRested = false;
	CanRest = false;
	CanSwitchTarget = true;
	InComboEndBuffer = false;
	ChoseCombo = false;
}

void AProjectOYGCharacter::BeginPlay()
{
	Super::BeginPlay();

	PotionMesh->SetVisibility(false, true);

	// Need to loop through to inventory find what is equipped

	WeaponClass = UnarmedWeaponClass;
	Weapon = GetWorld()->SpawnActor<AProjectOYGWeapon>(WeaponClass);
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
	Weapon->SetOwner(this);

	if(PickUpAudioComponent && PickUpSoundCue)
	{
		PickUpAudioComponent->SetSound(PickUpSoundCue);
	}
}

void AProjectOYGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(DeadScreenActive){return;}

	if(IsLockedOn)
	{
		if(!IsBackStabbing)
		{
			UpdateLockCamera(DeltaTime);
		}

		if(!ValidForLockOn(MinLockDist, CurrentLockedEnemy))
		{
			StopLockOn();
		}
	}

	if(CanStaminaRegen()) 
	{
		StaminaRegen(DeltaTime);
	}

	if(InDrinkRegen)
	{
		DrinkRegen(DeltaTime);
	}

	if(IsAttacking == 3 && !GetCharacterMovement()->IsFalling() && !IsLanding)
	{
		FallingAttackLand();
	}

	PoiseRegen(DeltaTime);

	CheckCancelSprint(DeltaTime);
}

void AProjectOYGCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectOYGCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectOYGCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &AProjectOYGCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AProjectOYGCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AProjectOYGCharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AProjectOYGCharacter::WaitForRoll);
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &AProjectOYGCharacter::Roll);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AProjectOYGCharacter::StopSprinting);

	PlayerInputComponent->BindAction("Climb", IE_Pressed, this, &AProjectOYGCharacter::Climb);
	PlayerInputComponent->BindAction("StopClimb", IE_Pressed, this, &AProjectOYGCharacter::StopClimb);

	PlayerInputComponent->BindAction("LockOn", IE_Pressed, this, &AProjectOYGCharacter::LockOn);

	PlayerInputComponent->BindAction("AdjustSight", IE_Pressed, this, &AProjectOYGCharacter::AdjustSight);

	PlayerInputComponent->BindAction("LightAttack", IE_Pressed, this, &AProjectOYGCharacter::LightAttack);
	PlayerInputComponent->BindAction("HeavyAttack", IE_Pressed, this, &AProjectOYGCharacter::HeavyAttack);

	PlayerInputComponent->BindAction("ToggleSheathRight", IE_Pressed, this, &AProjectOYGCharacter::ToggleSheathRight);

	PlayerInputComponent->BindAction("PickUp", IE_Pressed, this, &AProjectOYGCharacter::StartPickUp);

	PlayerInputComponent->BindAction("Drink", IE_Pressed, this, &AProjectOYGCharacter::StartDrink);

	PlayerInputComponent->BindAction("Rest", IE_Pressed, this, &AProjectOYGCharacter::StartRest);
	PlayerInputComponent->BindAction("StopRest", IE_Pressed, this, &AProjectOYGCharacter::StopRest);

	PlayerInputComponent->BindAction("Talk", IE_Pressed, this, &AProjectOYGCharacter::Talk);
}

void AProjectOYGCharacter::AddControllerYawInput(float Val) 
{
	if(IsLockedOn && CanSwitchTarget && !IsBackStabbing)
	{
		if(Val > 0.3f)
		{
			CanSwitchTarget = false;
			UE_LOG(LogTemp, Warning, TEXT("Turning Right"));
			SwitchLockRight();
		}
		else if(Val < -0.3f)
		{
			CanSwitchTarget = false;
			UE_LOG(LogTemp, Warning, TEXT("Turning Left"));
			SwitchLockLeft();
		}
	}
	else
	{
		Super::AddControllerYawInput(Val);
	}
}

void AProjectOYGCharacter::Jump() 
{
	if(PerformingAction()){return;}

	Super::Jump();
}

float AProjectOYGCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) 
{
	if((IsInvulnerable && (DamageAmount > 1.f)) || DeadScreenActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Take no damage"));
		return 0;
	} // iframes active, take damage "fails", && is to account for tick damage that should not be negated by iframes

	//UE_LOG(LogTemp, Warning, TEXT("Initial Health: %f, Damage To be dealt: %f" ), Health, DamageAmount);

	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	DamageToApply = DamageToApply - GetDamageReductionTotal(); // improve calculation 
	if(DamageToApply < 0.f) {DamageToApply = 0.f;}

	DamageToApply = FMath::Min(Health, DamageToApply);
	Health = Health - DamageToApply;
	//UE_LOG(LogTemp, Warning, TEXT("After Damage Health: %f"), Health);

	if(GetIsDead())
	{
		StopAllActions();

		DeadScreenActive = true;
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		GetCharacterMovement()->RotationRate.Yaw = 0.f;

		// player controller variable
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		// need to set previous character to destroy later
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorTickEnabled(false);
		PlayerController->UnPossess();

		// call game has ended to repawn player and enemies
		if(PlayerController != nullptr)
		{
			Cast<AProjectOYGPlayerController>(PlayerController)->PlayerDied();
		}
	}
	else
	{
		CurrentPoise = FMath::Max(0.f, CurrentPoise - PoiseDmgTaken);
		UE_LOG(LogTemp, Warning, TEXT("Poise: %f, PoiseDmgTaken: %f"), CurrentPoise, PoiseDmgTaken);
		if(CurrentPoise <= 0.01 && PoiseDmgTaken >= 0)
		{
			//if(IsAttacking != 0){StopAttack();}
			//if(IsRolling){StopRolling();}
			// Need stop all actions function

			IsStaggered = true;
			StopAllActions();
			GetWorldTimerManager().SetTimer(UnusedHandleTwo, this, &AProjectOYGCharacter::StopStagger, StaggerDuration, false);
			CurrentPoise = GetPoiseTotal();
		}
	}
	PoiseDmgTaken = 0.f;

	return DamageToApply;
}

bool AProjectOYGCharacter::GetIsDead() const
{
	if(Health <= 0.f)
	{
		return true;
	}

	return false;
}

bool AProjectOYGCharacter::GetIsStaggered() const
{
	return IsStaggered;
	//return false;
}

float AProjectOYGCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

float AProjectOYGCharacter::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

AProjectOYGWeapon* AProjectOYGCharacter::GetWeapon() const
{
	return Weapon;
}

int AProjectOYGCharacter::GetIsAttacking() const
{
	return IsAttacking;
}

bool AProjectOYGCharacter::GetIsBackStabbing() const
{
	return IsBackStabbing;
}

bool AProjectOYGCharacter::GetIsComboing() const
{
	return AttackQueued;
}

bool AProjectOYGCharacter::GetIsPickingUp() const
{
	return IsPickingUp;
}

bool AProjectOYGCharacter::GetIsSheathing() const
{
	return IsSheathing;
}

bool AProjectOYGCharacter::GetIsUnSheathing() const
{
	return IsUnSheathing;
}

bool AProjectOYGCharacter::GetIsClimbingLedge() const
{
	return IsClimbingLedge;
}

bool AProjectOYGCharacter::GetIsDrinking() const
{
	return IsDrinking;
}

int AProjectOYGCharacter::GetHealsLeft() const
{
	return Heals;
}

bool AProjectOYGCharacter::GetIsLockedOn() const
{
	return IsLockedOn;
}

bool AProjectOYGCharacter::GetIsOverlapping() const
{
	return IsOverlapping;
}

bool AProjectOYGCharacter::GetIsOverlappingTwo() const
{
	if(IsClimbing || IsResting)
	{
		return true;
	}

	return false;
}

FString AProjectOYGCharacter::GetOverlapMessage() const
{
	FString Message = TEXT("");
	if(CanClimb)
	{
		Message = TEXT("Press 'Interact' To Climb");
	}
	else if(CanPickUp)
	{
		Message = TEXT("Press 'Interact' To Pick Up");
	}
	else if(IsResting && !IsRested)
	{
		Message = TEXT("Press 'Interact' To Rest");
	}
	else if(CanRest)
	{
		Message = TEXT("Press 'Interact' To Use");
	}
	else if(CanTalk)
	{
		Message = TEXT("Press 'Interact' To Talk To");
	}
	return Message;
}

FString AProjectOYGCharacter::GetOverlapMessageSecondary() const
{
	FString Message = TEXT("");
	if(IsClimbing)
	{
		Message = TEXT("Press 'Back' To Get Off");
	}
	else if(IsResting)
	{
		Message = TEXT("Press 'Back' To Leave");
	}
	return Message;
}

FString AProjectOYGCharacter::GetSubtitles() const
{
	if(NPCActor)
	{
		return NPCActor->GetSubtitles();
	}
	return FString(" ");
}

void AProjectOYGCharacter::AddToInventory(AProjectOYGItem* Item) 
{
	UE_LOG(LogTemp, Warning, TEXT("Added %s to inventory"), *Item->GetName());
	
	Cast<AProjectOYGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->AddToInventory(Item);
}

void AProjectOYGCharacter::SetPoiseDmgTaken(float PoiseDmg) 
{
	PoiseDmgTaken = PoiseDmg;
}

void AProjectOYGCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	if(IsLockedOn)
	{
		if(CanSwitchTarget)
		{
			if(Rate > 0.3f)
			{
				CanSwitchTarget = false;
				UE_LOG(LogTemp, Warning, TEXT("Switching Right"));
				SwitchLockRight();
			}
			else if(Rate < -0.3f)
			{
				CanSwitchTarget = false;
				UE_LOG(LogTemp, Warning, TEXT("Switching Left"));
				SwitchLockLeft();
			}
		}
	}
	else
	{

		AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
	}
}

void AProjectOYGCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AProjectOYGCharacter::MoveForward(float Value)
{
	if(IsAttacking != 0 || IsRolling || InRollCooldown || IsStaggered || IsPickingUp || IsDrinking || IsResting || IsClimbingLedge || GetCharacterMovement()->IsFalling()){return;}

	if ((Controller != NULL) && (Value != 0.0f) && !(GetMovementComponent()->IsFalling()) && !IsClimbing)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
	else if(IsClimbing && !IsStartClimbing)
	{
		if(GetCharacterMovement()->IsMovingOnGround() && Value < -0.1f)
		{
			IsClimbing = false;
		}


		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Z);
		if(!OnLedge || Value < 0)
		{
			AddMovementInput(Direction, Value);
		}
		if(OnLedge && Value > 0 && !IsClimbingLedge)
		{
			ClimbLedge();
		}
	}
}

void AProjectOYGCharacter::MoveRight(float Value)
{
	if(IsAttacking != 0 || IsRolling || InRollCooldown || IsStaggered || IsPickingUp || IsDrinking || IsResting || IsClimbingLedge || GetCharacterMovement()->IsFalling()){return;}

	if ((Controller != NULL) && (Value != 0.0f) && !(GetMovementComponent()->IsFalling()) && !IsClimbing)
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
	else if(IsClimbing && !IsStartClimbing)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AProjectOYGCharacter::WaitForRoll() 
{
	if(PerformingAction()){ return; }

	CanRoll = true;
	IsInvulnerable = true;

	if(IsLockedOn)
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->RotationRate.Yaw = 10000;
	}
		
	GetWorldTimerManager().SetTimer(UnusedHandleTwo, this, &AProjectOYGCharacter::Sprint, SprintStartUp, false);
	//IsSprinting = false;
}

void AProjectOYGCharacter::Sprint() 
{
	if(PerformingAction()){return;}
	IsInvulnerable = false;

	CanRoll = false;
	// IDEA: Call Sprint on holding button button only activate sprint if !GetLastMovementInputVector().IsZero()

	GetCharacterMovement()->RotationRate.Yaw = 800.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
	IsSprinting = true;

	//GetCharacterMovement()->JumpZVelocity = SprintJumpHeight; // new value
}

void AProjectOYGCharacter::StopSprinting() 
{
	if(IsSprinting)
	{
		StaminaActionTriggered = true;
		if(IsLockedOn)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
			GetCharacterMovement()->MaxWalkSpeed = LockOnSpeed;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
		}
	}
	IsSprinting = false;

	//GetCharacterMovement()->JumpZVelocity = InitialJumpHeight; // back to original value
}

void AProjectOYGCharacter::CheckCancelSprint(float DeltaTime) 
{
	if(IsSprinting)
	{
		if(Stamina <= 0.f || GetCharacterMovement()->IsFalling() || GetCharacterMovement()->IsFlying() || GetLastMovementInputVector().IsZero())
		{
			StopSprinting();
		}
		if(Stamina > 0.f)
		{
			Stamina = Stamina - (SprintStaminaCost * DeltaTime);
		}
	}
}

void AProjectOYGCharacter::Roll() 
{
	if(!PerformingAction() && CanRoll && !GetLastMovementInputVector().IsZero())
	{
		Stamina = Stamina - RollStaminaCost;
		StaminaActionTriggered = true;

		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;

		IsRolling = true;
		IsInvulnerable = true;
		GetCharacterMovement()->BrakingFrictionFactor = 0.f;
		GetCharacterMovement()->GroundFriction = 0.f;
		GetCharacterMovement()->BrakingDecelerationWalking = RollDeceleration;
		GetCharacterMovement()->BrakingDecelerationFalling = RollDeceleration;

		LaunchCharacter(FVector(GetActorForwardVector().X, GetActorForwardVector().Y, 0).GetSafeNormal() * RollSpeed, true, true);
		InRollCooldown = true;

		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGCharacter::StopRolling, RollStop, false);
	}
}

void AProjectOYGCharacter::StopRolling() 
{
	GetCharacterMovement()->RotationRate.Yaw = 800.f;
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->BrakingFrictionFactor = InitialBrakingFactor;
	GetCharacterMovement()->GroundFriction = InitialGroundFriction;
	GetCharacterMovement()->BrakingDecelerationWalking = InitialGroundDeceleration;
	GetCharacterMovement()->BrakingDecelerationFalling = InitialAirDeceleration;
	IsInvulnerable = false;
	IsRolling = false;

	if(IsLockedOn)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	
	GetWorldTimerManager().SetTimer(UnusedHandleThree, this, &AProjectOYGCharacter::ResetRoll, RollCooldown, false);
}

void AProjectOYGCharacter::ResetRoll() 
{
	InRollCooldown = false;
}

void AProjectOYGCharacter::StartClimb() 
{
	IsOverlapping = false;
	IsStartClimbing = false;
}

void AProjectOYGCharacter::Climb() 
{
	if(ClimbActor == nullptr || !ClimbActor->IsA(AProjectOYGClimbSurface::StaticClass()))
	{
		return;
	}

	if(CanClimb && !PerformingAction() && !IsLockedOn)
	{
		IsOverlapping = false;
		SetActorRotation(FRotator(GetActorRotation().Pitch, ClimbActor->GetActorRotation().Yaw, GetActorRotation().Roll));
		//GetController()->SetControlRotation(ClimbActor->GetActorRotation()/*FRotator(GetController()->GetControlRotation().Pitch, ClimbActor->GetActorRotation().Yaw, GetController()->GetControlRotation().Roll*/));
		GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
		GetCharacterMovement()->SetMovementMode(MOVE_Flying); 
		SheathRight();
		IsStartClimbing = true;
		IsClimbing = true;

		GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGCharacter::StartClimb, ClimbStartTime, false);
	}
}

void AProjectOYGCharacter::ClimbLedge() 
{
	IsClimbingLedge = true;
	UE_LOG(LogTemp, Warning, TEXT("climb ledge start"));
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &AProjectOYGCharacter::StopClimbLedge, ClimbLedgeTime, false);
}

void AProjectOYGCharacter::StopClimb() 
{
	if(IsClimbing && !IsStartClimbing && !IsClimbingLedge)
	{
		if(CanClimb)
		{
			IsOverlapping = true;
		}

		GetCharacterMovement()->bOrientRotationToMovement = true;
		IsClimbing = false;

		if (GetCharacterMovement()->IsFlying() == true)
		{
			// If we are not flying already then set our movement mode so that we are  
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			UnSheathRight();
		}
	}
}

void AProjectOYGCharacter::StopClimbLedge() 
{
	SetActorLocation(Cast<AProjectOYGClimbSurface>(ClimbActor)->GetTopLocation()); 
	IsClimbingLedge = false;

	StopClimb();
}

void AProjectOYGCharacter::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AProjectOYGItem::StaticClass()))
	{
		CanPickUp = true;
		ItemActor = Cast<AProjectOYGItem>(OtherActor);
		IsOverlapping = true;
	}
	else if(OtherActor->IsA(AProjectOYGSavePoint::StaticClass()))
	{
		CanRest = true;
		SaveActor = Cast<AProjectOYGSavePoint>(OtherActor);
		IsOverlapping = true;
	}
	else if(OtherActor->IsA(AProjectOYGClimbSurface::StaticClass()))
	{
		CanClimb = true;

		ClimbActor = OtherActor;
		IsOverlapping = true;
		if(OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel2) // ECC_GameTraceChanne3 refers to Ledge collision object type
		{
			UE_LOG(LogTemp, Warning, TEXT("overlapping ledge"));
			OnLedge = true;
		}
	}
	else if(OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel4)
	{
		NPCActor = Cast<AProjectOYGEnemy_NPC>(OtherActor);
		CanTalk = true;
		IsOverlapping = true;
	}
	else if(OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel1) // ECC_GameTraceChannel refers to BackStab collision object type
	{
		BackStabOverlap = true;
		BackStabEnemy = Cast<AProjectOYGEnemy_Humanoid>(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("backstab actor set"));
	}
}

void AProjectOYGCharacter::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	IsOverlapping = false;
	if(OtherActor->IsA(AProjectOYGItem::StaticClass()))
	{
		CanPickUp = false;
	}
	else if(OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel1)
	{
		BackStabOverlap = false;
	}
	else if(OtherActor->IsA(AProjectOYGSavePoint::StaticClass()))
	{
		CanRest = false;
	}
	else if(OtherActor->IsA(AProjectOYGClimbSurface::StaticClass()) && !IsClimbingLedge)
	{
		CanClimb = false;

		ClimbActor = nullptr;
		IsClimbing = false;
		if(OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel2) // ECC_GameTraceChanne3 refers to Ledge collision object type
		{
			UE_LOG(LogTemp, Warning, TEXT("overlapping ledge"));
			OnLedge = false;
		}
	}
	else if(OtherComp->GetCollisionObjectType() == ECC_GameTraceChannel4)
	{
		CanTalk = false;
	}

	// CanClimb = false;
	// OnLedge = false;
	// IsClimbing = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	if (GetCharacterMovement()->IsFlying() == true && !IsClimbingLedge)
	{
		// If we are not flying already then set our movement mode so that we are  
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		UnSheathRight();
	}
	//ClimbActor = nullptr;
}

void AProjectOYGCharacter::LockOn() 
{	
	if(IsClimbing || IsBackStabbing){return;}

	if(!IsLockedOn)
	{
		NearestEnemy = nullptr;
		NearestEnemyDist = MinLockDist;
		//in cpp
		for (TObjectIterator<AProjectOYGEnemy> Itr; Itr; ++Itr)
		{
			if(ValidForLockOn(NearestEnemyDist, *Itr))
			{
				NearestEnemyDist = Itr->DistanceFromPlayer();
				NearestEnemy = *Itr;
			}
		}

		if(NearestEnemy)
		{
			CurrentLockedEnemyDist = NearestEnemyDist;
			CurrentLockedEnemy = NearestEnemy;
			CurrentLockedEnemy->TargetLockOn();
			UE_LOG(LogTemp, Warning, TEXT("success on %s"), *NearestEnemy->GetName());
			IsLockedOn = true;
			GetCharacterMovement()->MaxWalkSpeed = LockOnSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("nothing in range/sight"));
		}

	}
	else // Unlock
	{
		StopLockOn();
	}
}

void AProjectOYGCharacter::SwitchLockRight() 
{
	// Too much repeated code in SwitchLockRight SwitchLockLeft and LockOn

	FVector RightVector = GetActorRightVector();
	//UE_LOG(LogTemp, Warning, TEXT("RightVector:    (%f, %f, %f)"), RightVector.X,  RightVector.Y, RightVector.Z);
	
	NearestEnemyDist = MinLockDist;

	for (TActorIterator<AProjectOYGEnemy> Itr(GetWorld()); Itr; ++Itr)
	{
		if(ValidForLockOn(NearestEnemyDist, *Itr) && *Itr != CurrentLockedEnemy)
		{
			FVector EnemyDirection = Itr->GetActorLocation() - CurrentLockedEnemy->GetActorLocation();
			EnemyDirection.Normalize();
			if(EnemyDirection.Equals(RightVector, 0.95f) && *Itr != CurrentLockedEnemy)
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s: (%f, %f, %f)"), *Itr->GetName(), EnemyDirection.X,  EnemyDirection.Y, EnemyDirection.Z);
				NearestEnemyDist = Itr->DistanceFromPlayer();
				NearestEnemy = *Itr;
			}
		}
	}

	if(NearestEnemy != CurrentLockedEnemy)
	{
		CurrentLockedEnemy->TargetLockOff(); //LockOff old enemy

		CurrentLockedEnemyDist = NearestEnemyDist;
		CurrentLockedEnemy = NearestEnemy;
		CurrentLockedEnemy->TargetLockOn();
		UE_LOG(LogTemp, Warning, TEXT("success on %s"), *NearestEnemy->GetName());
		IsLockedOn = true;
		GetCharacterMovement()->MaxWalkSpeed = LockOnSpeed;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;

		GetWorldTimerManager().SetTimer(SwitchTimer, this, &AProjectOYGCharacter::SwitchCooldownOver, SwitchTargetCoolDown, false);
	}
	else
	{
		CanSwitchTarget = true;
		UE_LOG(LogTemp, Warning, TEXT("nothing to switch to"));
	}
}

void AProjectOYGCharacter::SwitchLockLeft() 
{
	FVector LeftVector = GetActorRightVector() * -1;
	//UE_LOG(LogTemp, Warning, TEXT("LeftVector:    (%f, %f, %f)"), LeftVector.X,  LeftVector.Y, LeftVector.Z);
	
	NearestEnemyDist = MinLockDist;

	for (TActorIterator<AProjectOYGEnemy> Itr(GetWorld()); Itr; ++Itr)
	{
		if(ValidForLockOn(NearestEnemyDist, *Itr) && *Itr != CurrentLockedEnemy)
		{
			FVector EnemyDirection = Itr->GetActorLocation() - CurrentLockedEnemy->GetActorLocation();
			EnemyDirection.Normalize();
			if(EnemyDirection.Equals(LeftVector, 0.95f) && *Itr != CurrentLockedEnemy)
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s: (%f, %f, %f)"), *Itr->GetName(), EnemyDirection.X,  EnemyDirection.Y, EnemyDirection.Z);
				NearestEnemyDist = Itr->DistanceFromPlayer();
				NearestEnemy = *Itr;
			}
		}
	}

	if(NearestEnemy != CurrentLockedEnemy)
	{
		CurrentLockedEnemy->TargetLockOff(); //LockOff old enemy

		CurrentLockedEnemyDist = NearestEnemyDist;
		CurrentLockedEnemy = NearestEnemy;
		CurrentLockedEnemy->TargetLockOn();
		UE_LOG(LogTemp, Warning, TEXT("success on %s"), *NearestEnemy->GetName());
		IsLockedOn = true;
		GetCharacterMovement()->MaxWalkSpeed = LockOnSpeed;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;

		GetWorldTimerManager().SetTimer(SwitchTimer, this, &AProjectOYGCharacter::SwitchCooldownOver, SwitchTargetCoolDown, false);
	}
	else
	{
		CanSwitchTarget = true;
		UE_LOG(LogTemp, Warning, TEXT("nothing to switch to"));
	}
}

void AProjectOYGCharacter::StopLockOn() 
{
	IsLockedOn = false;
	GetCharacterMovement()->RotationRate.Yaw = 800.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	CurrentLockedEnemy->TargetLockOff();
	CurrentLockedEnemy = nullptr;
	GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	//UE_LOG(LogTemp, Warning, TEXT("unlock"));
}

void AProjectOYGCharacter::SwitchCooldownOver() 
{
	CanSwitchTarget = true;
}

void AProjectOYGCharacter::UpdateLockCamera(float DeltaTime) 
{
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(CameraBoom->GetComponentLocation(), CurrentLockedEnemy->GetActorLocation());
	FRotator NewRot = FMath::RInterpTo(PlayerRot, GetController()->GetControlRotation(), GetWorld()->GetDeltaSeconds(), LockOnInterp * DeltaTime);
	//AddControllerYawInput(NewRot.Yaw);
	GetController()->SetControlRotation(FRotator(GetController()->GetControlRotation().Pitch, NewRot.Yaw, GetController()->GetControlRotation().Roll));
}

bool AProjectOYGCharacter::ValidForLockOn(float Distance, class AProjectOYGEnemy* CurrentEnemy) 
{
	// Set up
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	// Raycast
	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		CurrentEnemy->GetActorLocation(),
		this->GetActorLocation(), // enemy loc
		(FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldStatic), FCollisionObjectQueryParams(ECollisionChannel::ECC_WorldDynamic), FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn)),
		TraceParams
	);

	if(CurrentEnemy == Hit.GetActor())
	{
		if(CurrentEnemy->DistanceFromPlayer() < Distance)
		{
			FRotator Direction = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), CurrentEnemy->GetActorLocation());
			
			float Difference = FMath::Abs(GetController()->GetControlRotation().GetNormalized().Yaw - Direction.GetNormalized().Yaw);
			if(Difference < LockOnPeripheral || Difference > 350.f) // "350" - unusual error case
			{
				return true;
			}
		}
	}
	return false;
}

void AProjectOYGCharacter::AdjustSight() 
{
	// For Smoothness Use Lockon logic(interp)
	// FRotator NewRot = FMath::RInterpTo(GetActorRotation(), GetController()->GetControlRotation(), GetWorld()->GetDeltaSeconds(), LockOnInterp);
	// //AddControllerYawInput(NewRot.Yaw);
	// GetController()->SetControlRotation(FRotator(GetController()->GetControlRotation().Pitch, NewRot.Yaw, GetController()->GetControlRotation().Roll));

	GetController()->SetControlRotation(GetActorRotation());
}

bool AProjectOYGCharacter::CanStaminaRegen() 
{
	if(Stamina >= MaxStamina)
	{
		if(Stamina > MaxStamina)
		{
			Stamina = MaxStamina;
		}

		return false;
	}
	else if(InStaminaCooldown)
	{
		return false;
	}
	else if(StaminaActionTriggered)
	{
		StaminaActionTriggered = false;
		InStaminaCooldown = true;

		GetWorldTimerManager().SetTimer(UnusedHandleFour, this, &AProjectOYGCharacter::WaitForStaminaCooldown, StaminaCooldown, false);

		return false;
	}
	else if(IsSprinting || (IsAttacking > 0))
	{
		return false;
	}

	return true;
}

void AProjectOYGCharacter::WaitForStaminaCooldown()
{
	InStaminaCooldown = false;
}

void AProjectOYGCharacter::PoiseRegen(float DeltaTime) 
{
	if(CurrentPoise < GetPoiseTotal())
	{
		CurrentPoise = FMath::Min(CurrentPoise + PoiseRegenAmount, GetPoiseTotal());
	}
	else if(CurrentPoise > GetPoiseTotal())
	{
		CurrentPoise = GetPoiseTotal();
	}
}

void AProjectOYGCharacter::StaminaRegen(float DeltaTime) 
{
	if(Stamina <= 0.f)
	{
		Stamina = 0.f;
		CanRoll = false;
	}

	Stamina = Stamina + (StaminaRegenAmount * DeltaTime);
}

void AProjectOYGCharacter::LightAttack() 
{
	if(GetCharacterMovement()->IsFalling() && IsAttacking == 0 && GetVelocity().Z < -450.f && WeaponClass != nullptr) 
	{
		FallingAttack();
		return;
	}
	else if(BackStabEnemy != nullptr && BackStabOverlap && !PerformingAction()  && WeaponClass != nullptr)
	{
		if(BackStabEnemy->IsBackStabable())
		{
			BackStab();
			return;
		}
	}

	if(RightSheathed){return;}

	if(IsAttacking == 1 && !InComboEndBuffer)
	{
		AttackQueued = true;
	}

	if((!PerformingAction() || InComboEndBuffer) && WeaponClass != nullptr)
	{
		ChoseCombo = true;

		PlayAnimMontage(AttackMontage, 1.f, FName("Light"));
		IsAttacking = 1;
		UE_LOG(LogTemp, Warning, TEXT("attacking"));
		if(!InComboEndBuffer)
		{
			Stamina = Stamina - Weapon->GetLightAttackStaminaCost();\
		}
		StaminaActionTriggered = true;

		if(InComboEndBuffer) // if true weapon is comboing
		{
			LightAttackCombo();
			return;
		}
		else
		{
			Weapon->StartAttack(1, false);
		}
		

		if(Weapon->GetCombos() && Stamina >= 0)
		{
			GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::LightAttackEnd, Weapon->GetLightAttackDuration(), false);
		}
		else
		{
			GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::StopAttack, Weapon->GetLightAttackDuration(), false);
		}
	}
}

void AProjectOYGCharacter::LightAttackEnd() 
{
	if(AttackQueued)
	{
		UE_LOG(LogTemp, Warning, TEXT("Buffered"));
		StopAttack();
		return;
	}
	ChoseCombo = false;
	InComboEndBuffer = true;
	GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::StopLightAttack, Weapon->GetComboEndBufferDuration(), false);
}

void AProjectOYGCharacter::LightAttackCombo() 
{
	AttackQueued = false;
	PlayAnimMontage(AttackMontage, 1.f, FName("LightCombo"));
	IsAttacking = 1;
	
	UE_LOG(LogTemp, Warning, TEXT("Combo attack"));
	Stamina = Stamina - Weapon->GetLightAttackStaminaCost();
	StaminaActionTriggered = true;

	Weapon->StartAttack(1, true);

	if(Weapon->GetCombos() && Stamina >= 0)
	{
		GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::LightAttackEnd, Weapon->GetLightAttackDuration(), false);
	}
	else
	{
		GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::StopAttack, Weapon->GetLightAttackDuration(), false);
	}
}

void AProjectOYGCharacter::StopLightAttack() 
{
	if(ChoseCombo == false)
	{
		StopAttack();
	}
}

void AProjectOYGCharacter::BackStab() 
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	IsInvulnerable = true;
	IsAttacking = 5;
	IsBackStabbing = true;

	BackStabEnemy->BackStabFreeze(Weapon->GetBackStabDuration());

	SetActorRotation(BackStabEnemy->GetActorRotation());
	FVector EnemyLocation = FVector(BackStabEnemy->GetActorLocation().X, BackStabEnemy->GetActorLocation().Y, this->GetActorLocation().Z);
	FVector BackStabOrientation = BackStabEnemy->GetActorForwardVector();
	BackStabOrientation.Normalize();
	SetActorLocation(EnemyLocation + (BackStabOrientation * -Weapon->GetBackStabDistance()));

	GetWorldTimerManager().SetTimer(UnusedHandleNine, this, &AProjectOYGCharacter::BackStabHit, Weapon->GetBackStabHitTime(), false);

	GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::StopAttack, Weapon->GetBackStabDuration(), false);
	UE_LOG(LogTemp, Warning, TEXT("backstabing"));
}

void AProjectOYGCharacter::BackStabHit() 
{
	BackStabEnemy->BackStabTakeDamage(Weapon->GetBackStabDamage());
}

void AProjectOYGCharacter::HeavyAttack() 
{
	if(RightSheathed){return;}

	if(!PerformingAction() && WeaponClass != nullptr)
	{
		IsAttacking = 2;
		PlayAnimMontage(AttackMontage, 1.f, FName("Heavy"));
		UE_LOG(LogTemp, Warning, TEXT("attacking"));
		Stamina = Stamina - Weapon->GetHeavyAttackStaminaCost();
		StaminaActionTriggered = true;

		Weapon->StartAttack(2, false);

		GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::StopAttack, Weapon->GetHeavyAttackDuration(), false);
	}
}

void AProjectOYGCharacter::FallingAttack() 
{
	GetCharacterMovement()->StopMovementImmediately();
	IsAttacking = 3;
	UE_LOG(LogTemp, Warning, TEXT("attacking"));
	Stamina = Stamina - Weapon->GetHeavyAttackStaminaCost();
	StaminaActionTriggered = true;

	Weapon->StartAttack(3, false);

	//GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::StopAttack, Weapon->GetHeavyAttackDuration(), false);
}

void AProjectOYGCharacter::FallingAttackLand() 
{
	UE_LOG(LogTemp, Warning, TEXT("land"));
	IsLanding = true;
	GetWorldTimerManager().SetTimer(UnusedHandleFive, this, &AProjectOYGCharacter::StopAttack, FallingAtkEndLag, false);
}

void AProjectOYGCharacter::StopAttack() 
{
	InComboEndBuffer = false;
	StaminaActionTriggered = true;
	IsLanding = false;
	IsInvulnerable = false;

	if(IsBackStabbing)
	{
		IsAttacking = 0;

		IsBackStabbing = false;
		if(IsLockedOn)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
		}
	}

	if(!AttackQueued ||  Stamina <= 0.f)
	{
		IsAttacking = 0;
		AttackQueued = false;
		if(IsLockedOn)
		{
			GetCharacterMovement()->bOrientRotationToMovement = false;
			bUseControllerRotationYaw = true;
		}
	}
	else
	{
		LightAttackCombo();
	}

	UE_LOG(LogTemp, Warning, TEXT("stop attacking"));
}

void AProjectOYGCharacter::StopStagger() 
{
	IsStaggered = false;
}

float AProjectOYGCharacter::GetDamageReductionTotal() 
{
	float total = 0.f;
	if(ArmorHead != nullptr){total += ArmorHead->GetDmgReduction();}
	if(ArmorChest != nullptr){total += ArmorChest->GetDmgReduction();}
	if(ArmorHands != nullptr){total += ArmorHands->GetDmgReduction();}
	if(ArmorLegs != nullptr){total += ArmorLegs->GetDmgReduction();}
	if(ArmorFeet != nullptr){total += ArmorFeet->GetDmgReduction();}
	UE_LOG(LogTemp, Warning, TEXT("Armor: %f"), total);
	return total;
}

float AProjectOYGCharacter::GetPoiseTotal() 
{
	float total = 0.f;
	if(ArmorHead != nullptr){total += ArmorHead->GetPoise();}
	if(ArmorChest != nullptr){total += ArmorChest->GetPoise();}
	if(ArmorHands != nullptr){total += ArmorHands->GetPoise();}
	if(ArmorLegs != nullptr){total += ArmorLegs->GetPoise();}
	if(ArmorFeet != nullptr){total += ArmorFeet->GetPoise();}
	return total;
}

void AProjectOYGCharacter::ToggleSheathRight() 
{
	if(!PerformingAction() && WeaponClass != nullptr)
	{
		if(!RightSheathed)
		{
			IsSheathing = true;
			GetWorldTimerManager().SetTimer(UnusedHandleSeven, this, &AProjectOYGCharacter::StartSheath, SheathStartTime, false);
		}
		else if(RightSheathed)
		{
			IsUnSheathing = true;
			GetWorldTimerManager().SetTimer(UnusedHandleSeven, this, &AProjectOYGCharacter::StartUnSheath, SheathStartTime, false);
		}
	}
}

void AProjectOYGCharacter::SheathRight() 
{
	if(WeaponClass != nullptr)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("SheathSocket"));
		RightSheathed = true;
	}
}

void AProjectOYGCharacter::UnSheathRight() 
{
	if(WeaponClass != nullptr)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		RightSheathed = false;
	}
}

void AProjectOYGCharacter::SheathLeft() 
{
	
}

void AProjectOYGCharacter::UnSheathLeft() 
{
	
}

void AProjectOYGCharacter::StartSheath() 
{
	GetWorldTimerManager().SetTimer(UnusedHandleSeven, this, &AProjectOYGCharacter::StopSheath, SheathEndTime, false);
	SheathRight();
}

void AProjectOYGCharacter::StartUnSheath() 
{
	GetWorldTimerManager().SetTimer(UnusedHandleSeven, this, &AProjectOYGCharacter::StopUnSheath, SheathEndTime, false);
	UnSheathRight();
}

void AProjectOYGCharacter::StopSheath() 
{
	IsSheathing = false;
}

void AProjectOYGCharacter::StopUnSheath() 
{
	IsUnSheathing = false;
}

void AProjectOYGCharacter::StartPickUp() 
{
	if(!PerformingAction() && CanPickUp && ItemActor != nullptr)
	{
		IsPickingUp = true;
		SheathRight();
		GetWorldTimerManager().SetTimer(UnusedHandleSix, this, &AProjectOYGCharacter::PickUp, StartPickUpTime, false);
	}
}

void AProjectOYGCharacter::PickUp() 
{
	ItemActor->ItemGrab();
	//Equip(ItemActor);

	Cast<AProjectOYGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->UpdateInventory();

	GetWorldTimerManager().SetTimer(UnusedHandleSix, this, &AProjectOYGCharacter::StopPickUp, EndPickUpTime, false);

	if(PickUpAudioComponent)
	{
		//PickUpAudioComponent->SetPitchMultiplier(FMath::RandRange(0.8f, 1.f));
		PickUpAudioComponent->Play(0.f);
	}
}

void AProjectOYGCharacter::StopPickUp() 
{
	IsPickingUp = false;
	UnSheathRight();
}

void AProjectOYGCharacter::Equip(AProjectOYGItem* Item) 
{
	if(Item == nullptr){return;}

	Cast<AProjectOYGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->FindAndUnequip(Item);

	if(Item->IsA(AProjectOYGItem_Weapon::StaticClass()) /*&& WeaponClass == nullptr*/)
	{
		if(Weapon != nullptr) {Weapon->Destroy();}
		UE_LOG(LogTemp, Warning, TEXT("weapon equipped"));
		WeaponClass = Cast<AProjectOYGItem_Weapon>(Item)->GetWeaponClass();
		if(WeaponClass == nullptr){return;}
		if(Weapon != nullptr){Weapon->Destroy();}
		Weapon = GetWorld()->SpawnActor<AProjectOYGWeapon>(WeaponClass);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		Weapon->SetOwner(this);
	}
	else if(Item->IsA(AProjectOYGItem_Armor::StaticClass()) /*&& WeaponClass == nullptr*/)
	{
		UE_LOG(LogTemp, Warning, TEXT("armor equipped"));
		if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 1) // Head
		{
			ArmorHeadClass = Cast<AProjectOYGItem_Armor>(Item)->GetArmorClass();
			if(ArmorHeadClass == nullptr){return;}
			if(ArmorHead != nullptr){ArmorHead->Destroy();}
			ArmorHead = GetWorld()->SpawnActor<AProjectOYGArmor>(ArmorHeadClass);
			if(ArmorHead == nullptr){return;}
			ArmorHead->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
			ArmorHead->SetOwner(this);
			ArmorHeadMesh = ArmorHead->GetMesh();
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 2) // Chest
		{
			ArmorChestClass = Cast<AProjectOYGItem_Armor>(Item)->GetArmorClass();
			if(ArmorChestClass == nullptr){return;}
			if(ArmorChest != nullptr){ArmorChest->Destroy();}
			ArmorChest = GetWorld()->SpawnActor<AProjectOYGArmor>(ArmorChestClass);
			if(ArmorChest == nullptr){return;}
			ArmorChest->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
			ArmorChest->SetOwner(this);
			ArmorChestMesh = ArmorChest->GetMesh();
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 3) // Hands
		{
			ArmorHandsClass = Cast<AProjectOYGItem_Armor>(Item)->GetArmorClass();
			if(ArmorHandsClass == nullptr){return;}
			if(ArmorHands != nullptr){ArmorHands->Destroy();}
			ArmorHands = GetWorld()->SpawnActor<AProjectOYGArmor>(ArmorHandsClass);
			if(ArmorHands == nullptr){return;}
			ArmorHands->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
			ArmorHands->SetOwner(this);
			ArmorHandsMesh = ArmorHands->GetMesh();
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 4) // Legs
		{
			ArmorLegsClass = Cast<AProjectOYGItem_Armor>(Item)->GetArmorClass();
			if(ArmorLegsClass == nullptr){return;}
			if(ArmorLegs != nullptr){ArmorLegs->Destroy();}
			ArmorLegs = GetWorld()->SpawnActor<AProjectOYGArmor>(ArmorLegsClass);
			if(ArmorLegs == nullptr){return;}
			if(NakedMesh == nullptr){return;}
			GetMesh()->SetSkeletalMesh(NakedMesh, false);
			ArmorLegs->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
			ArmorLegs->SetOwner(this);
			ArmorLegsMesh = ArmorLegs->GetMesh();
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 5) // Feet
		{
			ArmorFeetClass = Cast<AProjectOYGItem_Armor>(Item)->GetArmorClass();
			if(ArmorFeetClass == nullptr){return;}
			if(ArmorFeet != nullptr){ArmorFeet->Destroy();}
			ArmorFeet = GetWorld()->SpawnActor<AProjectOYGArmor>(ArmorFeetClass);
			if(ArmorFeet == nullptr){return;}
			ArmorFeet->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
			ArmorFeet->SetOwner(this);
			ArmorFeetMesh = ArmorFeet->GetMesh();
		}
	}

	Item->SetIsEquipped(true);
}

void AProjectOYGCharacter::UnEquip(AProjectOYGItem* Item) 
{
	if(Item == nullptr){return;}

	if(Item->IsA(AProjectOYGItem_Weapon::StaticClass()) /*&& WeaponClass == nullptr*/)
	{
		UE_LOG(LogTemp, Warning, TEXT("weapon unequipped"));
		if(Weapon != nullptr)
		{
			Weapon->Destroy();
			Weapon = nullptr;
			WeaponClass = nullptr;
		}
		WeaponClass = UnarmedWeaponClass;
		Weapon = GetWorld()->SpawnActor<AProjectOYGWeapon>(WeaponClass);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
		Weapon->SetOwner(this);
	}
	else if(Item->IsA(AProjectOYGItem_Armor::StaticClass()) /*&& WeaponClass == nullptr*/)
	{
		UE_LOG(LogTemp, Warning, TEXT("armor unequipped"));
		if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 1) // Head
		{
			if(ArmorHead != nullptr)
			{
				ArmorHead->Destroy();
				ArmorHead = nullptr;
				ArmorHeadClass = nullptr;
			}
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 2) // Chest
		{
			if(ArmorChest != nullptr)
			{
				ArmorChest->Destroy();
				ArmorChest = nullptr;
				ArmorChestClass = nullptr;
			}
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 3) // Hands
		{
			if(ArmorHands != nullptr)
			{
				ArmorHands->Destroy();
				ArmorHands = nullptr;
				ArmorHandsClass = nullptr;
			}
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 4) // Legs
		{
			if(ArmorLegs != nullptr)
			{
				ArmorLegs->Destroy();
				ArmorLegs = nullptr;
				ArmorLegsClass = nullptr;
			}
			GetMesh()->SetSkeletalMesh(DefaultMesh, false);
		}
		else if( Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 5) // Feet
		{
			if(ArmorFeet != nullptr)
			{
				ArmorFeet->Destroy();
				ArmorFeet = nullptr;
				ArmorFeetClass = nullptr;
			}
		}
	}

	Item->SetIsEquipped(false);
}

void AProjectOYGCharacter::StartDrink() 
{
	if(!PerformingAction() && Heals > 0)
	{
		Heals = Heals - 1;

		IsDrinking = true;
		SheathRight();
		GetWorldTimerManager().SetTimer(UnusedHandleSix, this, &AProjectOYGCharacter::Drink, DrinkStartTime, false);
		PotionMesh->SetVisibility(true, true);
	}
}

void AProjectOYGCharacter::Drink() 
{
	GetWorldTimerManager().SetTimer(UnusedHandleSix, this, &AProjectOYGCharacter::StopDrink, DrinkEndTime, false);

	if(Health != MaxHealth)
	{
		InDrinkRegen = true;
	}
	GetWorldTimerManager().SetTimer(UnusedHandleEight, this, &AProjectOYGCharacter::StopDrinkRegen, DrinkRegenTime, false);
}

void AProjectOYGCharacter::StopDrink() 
{
	PotionMesh->SetVisibility(false, true);
	UnSheathRight();
	IsDrinking = false;
}

void AProjectOYGCharacter::DrinkRegen(float DeltaTime) 
{
	if(((DrinkRegenAmount * DeltaTime) + Health) >= MaxHealth)
	{
		Health = MaxHealth;
	}
	else
	{
		Health = Health + (DrinkRegenAmount * DeltaTime);
	}
}

void AProjectOYGCharacter::StopDrinkRegen() 
{
	InDrinkRegen = false;
}

void AProjectOYGCharacter::StartRest() 
{
	if(!PerformingAction() && CanRest && SaveActor != nullptr)
	{
		IsResting = true;

		SetActorRotation(FRotator(SaveActor->GetActorRotation().Pitch, -SaveActor->GetActorRotation().Yaw, SaveActor->GetActorRotation().Roll));

		FVector SaveLocation = FVector(SaveActor->GetActorLocation().X, SaveActor->GetActorLocation().Y, this->GetActorLocation().Z);
		FVector SaveOrientation = SaveActor->GetActorForwardVector();
		SaveOrientation.Normalize();
		SetActorLocation(SaveLocation + (SaveOrientation * 74.f));

		SaveActor->Activate();

		Cast<AProjectOYGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->ChangeSpawnLocation(SaveLocation + (SaveOrientation * 74.f));
	}
	else if(IsResting && CanRest && !IsRested)
	{
		IsRested = true;
		IsOverlapping = false; // Still overlapping, but should no longer display message

		Heals = 3; // set to max heals
		Health = MaxHealth;

		SaveActor->Rest();

		Cast<AProjectOYGPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->RespawnEnemies();
	}
}

void AProjectOYGCharacter::StopRest() 
{
	if(IsResting)
	{
		IsResting = false;
		IsRested = false;
	}
}

void AProjectOYGCharacter::Talk() 
{
	if(CanTalk && !PerformingAction())
	{
		NPCActor->PlayDialogue();
		CanTalk = false;
		IsOverlapping = false; // idk about this
	}
}

void AProjectOYGCharacter::StopAllActions() 
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	if(IsClimbing){StopClimb();}
	if(IsDrinking){	StopDrink();
					StopDrinkRegen();}	
	if(IsSprinting){StopSprinting();}
	if(IsPickingUp){StopPickUp();}
	if(Weapon != nullptr)
	{
		if(IsAttacking != 0)
		{
			GetWorld()->GetTimerManager().ClearAllTimersForObject(Weapon);
			Weapon->DeactivateHitbox();
			StopAttack();
		}
		if(IsSheathing){StopSheath();}
		if(IsUnSheathing){StopUnSheath();}
	}
	InStaminaCooldown = false;
	InDrinkRegen = false;
}

bool AProjectOYGCharacter::PerformingAction() 
{
	if(IsAttacking != 0 || IsRolling || InRollCooldown || IsClimbing || IsClimbingLedge || Stamina <= 0.f || Health <= 0.f ||
	IsStaggered || IsPickingUp || IsSheathing || IsUnSheathing || IsDrinking || IsResting ||
	GetCharacterMovement()->IsFlying() || GetCharacterMovement()->IsFalling())
	{
		return true;
	}
	return false;
}

