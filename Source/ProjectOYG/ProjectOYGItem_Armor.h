// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGItem.h"
#include "ProjectOYGItem_Armor.generated.h"

class AProjectOYGArmor;
/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGItem_Armor : public AProjectOYGItem
{
	GENERATED_BODY()
	
	public:
		AProjectOYGItem_Armor();

		TSubclassOf<AProjectOYGArmor> GetArmorClass();
		int GetSlotID();

	private:
		UPROPERTY(EditDefaultsOnly)
			TSubclassOf<AProjectOYGArmor> ArmorClass;
		UPROPERTY(EditAnywhere)
			int SlotID;
};
