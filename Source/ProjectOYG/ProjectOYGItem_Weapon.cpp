// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGItem_Weapon.h"
#include "ProjectOYGWeapon.h"




AProjectOYGItem_Weapon::AProjectOYGItem_Weapon() 
{
    
}

TSubclassOf<AProjectOYGWeapon> AProjectOYGItem_Weapon::GetWeaponClass() 
{
    return WeaponClass;
}
