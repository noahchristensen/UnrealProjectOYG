// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ChangeSpeed.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "ProjectOYGEnemy.h"


UBTTask_ChangeSpeed::UBTTask_ChangeSpeed() 
{
    NodeName = TEXT("ChangeSpeed");
}

EBTNodeResult::Type UBTTask_ChangeSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
    AProjectOYGEnemy* Enemy = Cast<AProjectOYGEnemy>(OwnerComp.GetAIOwner()->GetPawn());
    if(Enemy == nullptr)
    {
        return EBTNodeResult::Failed;
    }

    Enemy->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

    UE_LOG(LogTemp, Warning, TEXT("SpeedChanged"));

    return EBTNodeResult::Succeeded;
}
