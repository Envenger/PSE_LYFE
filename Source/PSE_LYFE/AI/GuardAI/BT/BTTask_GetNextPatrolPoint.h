// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_GetNextPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API UBTTask_GetNextPatrolPoint : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

	UBTTask_GetNextPatrolPoint(const FObjectInitializer& ObjectInitializer);
	
	EBTNodeResult::Type ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditDefaultsOnly, Category = BBKeyNames)
	FText BBPatrolPointName;
};
