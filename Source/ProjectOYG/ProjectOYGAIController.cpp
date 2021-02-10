// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectOYGAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"

void AProjectOYGAIController::BeginPlay() 
{
    Super::BeginPlay();

    if(AIBehavior != nullptr)
    {
        RunBehaviorTree(AIBehavior);

        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if(PlayerPawn == nullptr){return;}

        if(GetBlackboardComponent() == nullptr){return;}

        //UE_LOG(LogTemp, Warning, TEXT("Num Keys: %f "), GetBlackboardComponent()->GetNumKeys());

        //if(GetBlackboardComponent()->GetValueAsVector(TEXT("StartLocation")) == nullptr){return;}

        //GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
        
        //GetBlackboardComponent()->SetValueAsVector(TEXT("PlayerLocation"), PlayerPawn->GetActorLocation());
    }
}

void AProjectOYGAIController::Tick(float DeltaSeconds) 
{
    Super::Tick(DeltaSeconds);
}
