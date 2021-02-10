// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_IsNotAttacking.h"
#include "AIController.h"
#include "ProjectOYGEnemy.h"

UBTDecorator_IsNotAttacking::UBTDecorator_IsNotAttacking()
{
    NodeName = TEXT("Is Not Attacking?");
}

bool UBTDecorator_IsNotAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const 
{
    AProjectOYGEnemy* Enemy = Cast<AProjectOYGEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if(Enemy == nullptr)
    {
        return false;
    }

    if(Enemy->GetIsAttacking() == 0)
    {
        return true;
    }

    return false;
}
