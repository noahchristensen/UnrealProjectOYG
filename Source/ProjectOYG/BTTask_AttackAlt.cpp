// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackAlt.h"
#include "AIController.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGEnemy_Humanoid.h"
#include "ProjectOYGEnemy_Rabbit.h"
#include "ProjectOYGEnemy_Slug.h"
#include "ProjectOYGEnemy_Bear.h"


UBTTask_AttackAlt::UBTTask_AttackAlt() 
{
    NodeName = TEXT("AttackAlt");
}

EBTNodeResult::Type UBTTask_AttackAlt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
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
        Cast<AProjectOYGEnemy_Humanoid>(Enemy)->HeavyAttack();
    }
    else if(Enemy->IsA(AProjectOYGEnemy_Rabbit::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Rabbit Attack"));
        Cast<AProjectOYGEnemy_Rabbit>(Enemy)->KickFrontStart();
    }
    else if(Enemy->IsA(AProjectOYGEnemy_Slug::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Slug Attack"));
        Cast<AProjectOYGEnemy_Slug>(Enemy)->SpitStart();
    }
    else if(Enemy->IsA(AProjectOYGEnemy_Bear::StaticClass()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Bear Attack"));
        Cast<AProjectOYGEnemy_Bear>(Enemy)->DiveStart();
    }

    return EBTNodeResult::Succeeded;
}

