// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "AI/GuardAI/GuardAIController.h"
#include "AI/GuardAI/GuardAICharacter.h"
#include "BTTask_PatrolPointReached.h"

UBTTask_PatrolPointReached::UBTTask_PatrolPointReached(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = "PatrolPointReached";
	WaitTime = 4.5f;
	RandomDeviation = 1.5;
}

EBTNodeResult::Type UBTTask_PatrolPointReached::ExecuteTask(class UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGuardAIController* OwnerController = Cast<AGuardAIController>((&OwnerComp)->GetAIOwner());
	AGuardAICharacter* OwnerPawn = Cast<AGuardAICharacter>(OwnerController->GetPawn());

/*
	const UBlackboardComponent* MyBlackboard = (&OwnerComp)->GetBlackboardComponent();

	if (MyBlackboard->GetKeyID(FName("bIsBotSelected")) != FBlackboard::InvalidKey)
	{
	//	bIsSelected = MyBlackboard->GetValue<UBlackboardKeyType_Bool>(FName("bIsBotSelected"));
	}*/

	FBTWaitTaskMemory* MyMemory = (FBTWaitTaskMemory*)NodeMemory;
	MyMemory->RemainingWaitTime = FMath::FRandRange(FMath::Max(0.0f, WaitTime - RandomDeviation), (WaitTime + RandomDeviation));

	return EBTNodeResult::InProgress;
}



