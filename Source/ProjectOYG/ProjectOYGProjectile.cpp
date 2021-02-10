// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGProjectile.h"
#include "ProjectOYGCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectOYGProjectile::AProjectOYGProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if(!RootComponent)
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if(!CollisionComponent)
	{
		// Use a sphere as a simple collision representation.
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

		// Set the sphere's collision profile name to "Projectile".
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));

		// Event called when component hits something.
		CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectOYGProjectile::OnHit);

		// Set the sphere's collision radius.
		CollisionComponent->InitSphereRadius(15.0f);

		// Set the root component to be the collision component.
		RootComponent = CollisionComponent;
	}

	if(!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if(!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
		ProjectileMeshComponent->SetupAttachment(RootComponent);
	}

	InitialLifeSpan = 3.0f;
	ProjectileDamage = 20.f;
}

// Called when the game starts or when spawned
void AProjectOYGProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectOYGProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectOYGProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) 
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile Hit"));
	if (OtherActor->IsA(AProjectOYGCharacter::StaticClass()))
	{
		Cast<AProjectOYGCharacter>(OtherActor)->SetPoiseDmgTaken(PoiseDmg);

	   //OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	   APawn* OwnerPawn = Cast<APawn>(GetOwner());
		if(OwnerPawn == nullptr){return;}
		AController* OwnerController = OwnerPawn->GetController();

		FPointDamageEvent DamageEvent; // nothing for now, can add for different hit effects possibly

		OtherActor->TakeDamage(ProjectileDamage, DamageEvent, OwnerController, this);
	}

	if(SplashEffect != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SplashEffect, GetActorLocation(), GetActorRotation());
	}

    Destroy();
}

void AProjectOYGProjectile::FireInDirection(const FVector& ShootDirection) 
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AProjectOYGProjectile::SetPoiseDmg(float PoiseDamage) 
{
	PoiseDmg = PoiseDamage;
}

