// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "AI/GuardAI/GuardAIController.h"
#include "AI/GuardAI/GuardAICharacter.h"
#include "AI/AITargetPoint.h"
#include "BTTask_GetClosestPatrolPoint.h"

EBTNodeResult::Type UBTTask_GetClosestPatrolPoint::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AGuardAIController* OwnerController = Cast<AGuardAIController>((&OwnerComp)->GetAIOwner());
	AGuardAICharacter* OwnerPawn = Cast<AGuardAICharacter>(OwnerController->GetPawn());
	if (OwnerPawn)
	{
		AAITargetPoint* PatrolPoint = nullptr;
		uint8 PatrolPointIndex = 0;
		OwnerPawn->GetClosestPatrolPoint(PatrolPointIndex, PatrolPoint);
		if (PatrolPoint)
		{
			(&OwnerComp)->GetBlackboardComponent()->SetValueAsObject(OwnerController->BBPatrolPointName, PatrolPoint);
			(&OwnerComp)->GetBlackboardComponent()->SetValueAsInt(OwnerController->BBPatrolPointIndexName, PatrolPointIndex);
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "ClosestPP");
			return EBTNodeResult::Succeeded;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "ClosestPP");
	return EBTNodeResult::Failed;
}


