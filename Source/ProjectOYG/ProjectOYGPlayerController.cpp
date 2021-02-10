// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectOYGCharacter.h"
#include "ProjectOYGEnemy.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectOYGItem.h"
#include "ProjectOYGItem_Weapon.h"
#include "ProjectOYGItem_Armor.h"
#include "EngineUtils.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

void AProjectOYGPlayerController::BeginPlay() 
{
    Super::BeginPlay();

    HUD = CreateWidget(this, HUDClass);
    if(HUD != nullptr)
    {
        HUD->AddToViewport();
    }

    LoseScreen = CreateWidget(this, LoseScreenClass);

    RespawnLocation = FVector(-125.626663f, 70.887115f, 219.281158f);
}

void AProjectOYGPlayerController::PlayerDied() 
{
    // if(LoseScreen != nullptr)
    // {
    //    LoseScreen->AddToViewport();
    // }

    //GetWorldTimerManager().SetTimer(RestartTimer, this, &AProjectOYGPlayerController::RespawnPlayer, RespawnDelay); // player is spawned at the end of respawn enemies
    GetWorldTimerManager().SetTimer(EnemyTimer, this, &AProjectOYGPlayerController::RespawnEnemies, RespawnDelay);
    GetWorldTimerManager().SetTimer(PlayerTimer, this, &AProjectOYGPlayerController::RespawnPlayer, RespawnDelay);
}

void AProjectOYGPlayerController::RespawnPlayer()
{
    // if(LoseScreen != nullptr)
    // {
    //     LoseScreen->RemoveFromViewport();
    // }

    GetWorld()->GetAuthGameMode()->RestartPlayerAtTransform(this, FTransform(RespawnLocation));
    ResetEquipment();
}

void AProjectOYGPlayerController::ChangeSpawnLocation(FVector NewSpawnLocation) 
{
    RespawnLocation = NewSpawnLocation;
}

void AProjectOYGPlayerController::ResetEquipment() 
{
    for (auto& Item : Inventory)
    {
        if(Item->GetIsEquipped() == true)
        {
            Cast<AProjectOYGCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->Equip(Item);
        }
    }
}

void AProjectOYGPlayerController::RespawnEnemies() 
{
    TArray<AProjectOYGEnemy*> WorldEnemies;

    for (TActorIterator<AProjectOYGEnemy> Itr(GetWorld()); Itr; ++Itr)
    {
        //Itr->Respawn();
        UE_LOG(LogTemp, Warning, TEXT("Enemy: %s "), *Itr->GetName());

        WorldEnemies.Add(*Itr);
    }

    UClass* EnemyClass;
    AAIController* EnemyAI;
    FVector EnemyLocation;
    FRotator EnemyRotation;

    for (auto& Enemy : WorldEnemies)
    {
        UE_LOG(LogTemp, Warning, TEXT("Enemy: %s "), *Enemy->GetName());

        if(Enemy->ShouldRespawn)
        {
            EnemyClass = Enemy->GetClass();
            EnemyAI = UAIBlueprintHelperLibrary::GetAIController(Enemy);
            EnemyLocation = Enemy->GetInitialLocation();
            EnemyRotation = Enemy->GetInitialRotation();

            Enemy->DestroyConstructedComponents();
            Enemy->Destroy();
            
            FActorSpawnParameters SpawnInfo;
            GetWorld()->SpawnActor<AProjectOYGEnemy>(EnemyClass, EnemyLocation, EnemyRotation, SpawnInfo);

        }
    }
}

void AProjectOYGPlayerController::AddToInventory(AProjectOYGItem* Item) 
{
	Inventory.Add(Item);
    UE_LOG(LogTemp, Warning, TEXT("In Player Controller, Added %s to inventory"), *Item->GetName());
}

TArray<AProjectOYGItem*> AProjectOYGPlayerController::GetInventory() 
{
    return Inventory;
}

void AProjectOYGPlayerController::FindAndUnequip(AProjectOYGItem* ItemToEquip) 
{
    if(ItemToEquip == nullptr){return;}

    for(auto& Item : Inventory)
    {
        if(Item->GetIsEquipped() == true)
        {
            if(ItemToEquip->IsA(AProjectOYGItem_Weapon::StaticClass()) && Item->IsA(AProjectOYGItem_Weapon::StaticClass()))
            {
                Item->SetIsEquipped(false);
            }
            else if(ItemToEquip->IsA(AProjectOYGItem_Armor::StaticClass()) && Item->IsA(AProjectOYGItem_Armor::StaticClass()))
            {
                UE_LOG(LogTemp, Warning, TEXT("armor equipped"));
                if(Cast<AProjectOYGItem_Armor>(ItemToEquip)->GetSlotID() == 1 && Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 1) // Head
                {
                    Item->SetIsEquipped(false);
                }
                else if(Cast<AProjectOYGItem_Armor>(ItemToEquip)->GetSlotID() == 2 && Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 2) // Chest
                {
                    Item->SetIsEquipped(false);
                }
                else if(Cast<AProjectOYGItem_Armor>(ItemToEquip)->GetSlotID() == 3 && Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 3) // Hands
                {
                    Item->SetIsEquipped(false);
                }
                else if(Cast<AProjectOYGItem_Armor>(ItemToEquip)->GetSlotID() == 4 && Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 4) // Legs
                {
                    Item->SetIsEquipped(false);
                }
                else if(Cast<AProjectOYGItem_Armor>(ItemToEquip)->GetSlotID() == 5 && Cast<AProjectOYGItem_Armor>(Item)->GetSlotID() == 5) // Feet
                {
                    Item->SetIsEquipped(false);
                }
            }
        }
    }
}

void AProjectOYGPlayerController::UpdateInventory() 
{
    OnUpdateInventory.Broadcast(Inventory);
}