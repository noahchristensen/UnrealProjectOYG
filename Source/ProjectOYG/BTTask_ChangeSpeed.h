// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ChangeSpeed.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API UBTTask_ChangeSpeed : public UBTTaskNode
{
	GENERATED_BODY()
	
	public:
		UBTTask_ChangeSpeed();
	
	protected:
		virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	public:
		UPROPERTY(EditAnywhere)
			float NewSpeed;
};
