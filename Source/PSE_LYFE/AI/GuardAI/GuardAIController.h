// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GuardAIController.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API AGuardAIController : public AAIController
{
	GENERATED_BODY()
	
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
	
	
};
