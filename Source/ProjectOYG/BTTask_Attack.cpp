// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Attack.h"
#include "AIController.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGEnemy_Humanoid.h"
#include "ProjectOYGEnemy_Rabbit.h"
#include "ProjectOYGEnemy_Slug.h"
#include "ProjectOYGEnemy_Bear.h"


UBTTask_Attack::UBTTask_Attack() 
{
    NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
    Super::ExecuteTask(OwnerComp, NodeMemory);

    if(OwnerComp.GetAIOwner() == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    AProjectOYGEnemy* Enemy = Cast<AProjectOYGEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if(Enemy == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    if(Enemy->IsA(AProjectOYGEnemy_Humanoid::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Humanoid Attack"));
        Cast<AProjectOYGEnemy_Humanoid>(Enemy)->LightAttack();
    }
    else if(Enemy->IsA(AProjectOYGEnemy_Rabbit::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Rabbit Attack"));
        Cast<AProjectOYGEnemy_Rabbit>(Enemy)->JumpStart();
    }
    else if(Enemy->IsA(AProjectOYGEnemy_Slug::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Slug Attack"));
        Cast<AProjectOYGEnemy_Slug>(Enemy)->StrikeStart();
    }
    else if(Enemy->IsA(AProjectOYGEnemy_Bear::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Bear Attack"));

        int Select = FMath::RandRange(1, 8);
        if(Select <= 3)
        {
            Cast<AProjectOYGEnemy_Bear>(Enemy)->OneSwipeStart();
        }
        else if(Select <= 5)
        {
            Cast<AProjectOYGEnemy_Bear>(Enemy)->TwoSwipeStart();
        }
        else if(Select <= 7)
        {
            Cast<AProjectOYGEnemy_Bear>(Enemy)->HeadStart();
        }
        else if(Select == 8)
        {
            Cast<AProjectOYGEnemy_Bear>(Enemy)->RoarStart();
        }
    }


    return EBTNodeResult::Succeeded;
}
