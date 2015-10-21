// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "AI/GuardAI/GuardAIController.h"
#include "AI/GuardAI/GuardAICharacter.h"
#include "AI/AITargetPoint.h"
#include "BTTask_GetNextPatrolPoint.h"

UBTTask_GetNextPatrolPoint::UBTTask_GetNextPatrolPoint(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "GetNextPatrolPoint";
}

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AGuardAIController* OwnerController = Cast<AGuardAIController>((&OwnerComp)->GetAIOwner());
	AGuardAICharacter* OwnerPawn = Cast<AGuardAICharacter>(OwnerController->GetPawn());
	if (OwnerPawn)
	{
		AAITargetPoint* PatrolPoint = nullptr;
		uint8 PatrolPointIndex = 0;
		const uint8 CurrentPatrolPointIndex = (&OwnerComp)->GetBlackboardComponent()->GetValueAsInt(OwnerController->BBPatrolPointIndexName);

		OwnerPawn->GetNextPatrolPoint(CurrentPatrolPointIndex, PatrolPointIndex, PatrolPoint);
		if (PatrolPoint)
		{
			(&OwnerComp)->GetBlackboardComponent()->SetValueAsObject(OwnerController->BBPatrolPointName, PatrolPoint);
			(&OwnerComp)->GetBlackboardComponent()->SetValueAsInt(OwnerController->BBPatrolPointIndexName, PatrolPointIndex);
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, "NextPP");
			return EBTNodeResult::Succeeded;
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "NextPP");
	return EBTNodeResult::Failed;
}


