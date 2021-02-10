// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectOYGEnemy.h"
#include "ProjectOYGEnemy_NPC.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTOYG_API AProjectOYGEnemy_NPC : public AProjectOYGEnemy
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* Collider;

	public:
		AProjectOYGEnemy_NPC();

		virtual void BeginPlay() override;

		void PlayDialogue();
		void PlayDialogueIndex(int i);

		FString GetSubtitles();

		UPROPERTY(EditAnywhere, Category = "Dialogue")
			TArray<FString> Dialogue;
		UPROPERTY(EditAnywhere, Category = "Dialogue")
			TArray<float> DialogueTime;

	private:
		int DialogueIndex;
		FTimerHandle DialogueHandle;
		FString Subtitles;

};
