// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackAlt.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API UBTTask_AttackAlt : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
		UBTTask_AttackAlt();
	
	protected:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
