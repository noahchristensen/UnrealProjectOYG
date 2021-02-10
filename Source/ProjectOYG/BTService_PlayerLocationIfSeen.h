// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_PlayerLocationIfSeen.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API UBTService_PlayerLocationIfSeen : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
	public:
		UBTService_PlayerLocationIfSeen();

	protected:
		virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds); 
	
	public:
	UPROPERTY(EditAnywhere)
		float ViewDistance = 1000.f;
	UPROPERTY(EditAnywhere)
		float FeelDistance = 100.f;
	UPROPERTY(EditAnywhere)
		float HearDistance = 1000.f;
	UPROPERTY(EditAnywhere)
		float LockOnPeripheral = 110.f;
	UPROPERTY(EditAnywhere)
		float AudibleSpeed = 320.f;
};
