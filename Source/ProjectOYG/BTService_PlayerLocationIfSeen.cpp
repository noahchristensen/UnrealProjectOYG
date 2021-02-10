// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_PlayerLocationIfSeen.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "AIController.h"


UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen() 
{
    NodeName = TEXT("Update Player Location If Seen");
}

void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if(PlayerPawn == nullptr)
    {
        return;
    }

    if(OwnerComp.GetAIOwner() == nullptr)
    {
        return;
    }

    float DistanceFromPlayer;
    // float ViewDistance = 1000.f;
    // float FeelDistance = 100.f;
    // float HearDistance = 1000.f;
    float Difference;
    // float LockOnPeripheral = 110.f;
    // float AudibleSpeed = 320.f;

    if(GetWorld())
	{
		if(GetWorld()->GetFirstPlayerController())
		{
			if(GetWorld()->GetFirstPlayerController()->GetPawn())
			{
				FVector PlayerCharacterLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
				DistanceFromPlayer =  FVector::Dist(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), PlayerCharacterLoc); // Wont Move Toward if player is out of range

                FRotator Direction = UKismetMathLibrary::FindLookAtRotation(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), PlayerCharacterLoc);	
                Difference = FMath::Abs(OwnerComp.GetAIOwner()->GetControlRotation().GetNormalized().Yaw - Direction.GetNormalized().Yaw); // Wont Move Toward if player is behind
			}
		}
	}

    if(OwnerComp.GetAIOwner()->LineOfSightTo(PlayerPawn) && DistanceFromPlayer < ViewDistance)
    {
        if((Difference < LockOnPeripheral || Difference > 350.f) || (DistanceFromPlayer < FeelDistance))
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerPawn->GetActorLocation());
        }
        else if((GetWorld()->GetFirstPlayerController()->GetPawn()->GetVelocity().Size() > AudibleSpeed) && (DistanceFromPlayer < HearDistance))
        {
            OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), PlayerPawn->GetActorLocation());
        }
        
    }
    else
    {
        OwnerComp.GetBlackboardComponent()->ClearValue(GetSelectedBlackboardKey());
    }
}
