// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectOYGPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateInventoryDelegate, const TArray<AProjectOYGItem*>&, InventoryItems);

class AProjectOYGItem;
class AProjectOYGEnemy;

UCLASS()
class PROJECTOYG_API AProjectOYGPlayerController : public APlayerController
{
	GENERATED_BODY()

	protected:
		virtual void BeginPlay() override;

	public:
		void PlayerDied();

		void RespawnPlayer();
		// Used when player rests at SavePoint's
		void ChangeSpawnLocation(FVector NewSpawnLocation);
		// Respawns all equipment on player that is listed in inventory as "IsEquipped"
		// Used after player respawns
		void ResetEquipment();

		void RespawnEnemies();

		// Adds item to inventory saved in player controller after player pick up
		void AddToInventory(AProjectOYGItem* Item);
		TArray<AProjectOYGItem*> GetInventory();

		// Finds any items of same type and unequips them if they are equipped
		// Used For equipping new items(replaces the old)
		void FindAndUnequip(AProjectOYGItem* Item);

		// For the inventory UI to keep track of inventory
		void UpdateInventory();
		UPROPERTY(BlueprintAssignable)
			FUpdateInventoryDelegate OnUpdateInventory;

	private:
		UPROPERTY(EditAnywhere)
			TSubclassOf<class UUserWidget> HUDClass;
		UPROPERTY(EditAnywhere)
			TSubclassOf<class UUserWidget> LoseScreenClass;
		
		UPROPERTY()
			UUserWidget *HUD;

		UPROPERTY(EditAnywhere)
			float RespawnDelay = 4.f;
		
		FTimerHandle EnemyTimer;
		FTimerHandle PlayerTimer;

		UUserWidget* LoseScreen;

		TArray<AProjectOYGItem*> Inventory;
		FVector RespawnLocation;
};
