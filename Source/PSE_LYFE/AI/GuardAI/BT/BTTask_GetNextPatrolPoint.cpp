// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "BTTask_GetNextPatrolPoint.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "GetNextPatrolPoint";
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::Succeeded;
}


