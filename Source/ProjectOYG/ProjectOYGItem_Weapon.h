// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGItem.h"
#include "ProjectOYGItem_Weapon.generated.h"


class AProjectOYGWeapon;
/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGItem_Weapon : public AProjectOYGItem
{
	GENERATED_BODY()
	
	public:
		AProjectOYGItem_Weapon();

		TSubclassOf<AProjectOYGWeapon> GetWeaponClass();
	private:
		UPROPERTY(EditDefaultsOnly)
			TSubclassOf<AProjectOYGWeapon> WeaponClass;
};
