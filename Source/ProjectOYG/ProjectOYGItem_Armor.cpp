// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGItem_Armor.h"
#include "ProjectOYGArmor.h"


AProjectOYGItem_Armor::AProjectOYGItem_Armor() 
{
    
}

TSubclassOf<AProjectOYGArmor> AProjectOYGItem_Armor::GetArmorClass() 
{
     return ArmorClass;
}

int AProjectOYGItem_Armor::GetSlotID() 
{
    return SlotID;
}