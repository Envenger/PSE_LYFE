// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GuardAIController.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API AGuardAIController : public AAIController
{
	GENERATED_BODY()

	UPROPERTY(Category = AI, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAIPerceptionComponent* PerceptionComp;

	UPROPERTY(Category = AI, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UAISenseConfig_Sight* AISight;
	
public:

	AGuardAIController();

	UPROPERTY( transient)
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	UBehaviorTreeComponent* BehaviorComp;

	UPROPERTY(EditDefaultsOnly, Category = Behavior)
	class UBehaviorTree* BotBehavior;

	/** Posses The character */
	virtual void Possess(class APawn* InPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category = BBKeyNames)
	FName BBPatrolPointName;

	UPROPERTY(EditDefaultsOnly, Category = BBKeyNames)
	FName BBPatrolPointIndexName;

	void SenseStuff(TArray<AActor*> testActors);
};
