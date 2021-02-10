// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsNotAttacking.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API UBTDecorator_IsNotAttacking : public UBTDecorator
{
	GENERATED_BODY()
	
	public:
		UBTDecorator_IsNotAttacking();

	protected:
		virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
