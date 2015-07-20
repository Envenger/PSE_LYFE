// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "BTTask_PatrolPointReached.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API UBTTask_PatrolPointReached : public UBTTask_Wait
{
	GENERATED_BODY()


	
	UBTTask_PatrolPointReached(const FObjectInitializer& ObjectInitializer);

	EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	
};
