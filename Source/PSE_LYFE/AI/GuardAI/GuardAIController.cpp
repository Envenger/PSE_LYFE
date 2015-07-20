// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "AI/GuardAI/GuardAICharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GuardAIController.h"

AGuardAIController::AGuardAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
}

void AGuardAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	BlackboardComp->InitializeBlackboard(*BotBehavior->BlackboardAsset);

	BehaviorComp->StartTree(*(BotBehavior));

	AGuardAICharacter* Bot = Cast<AGuardAICharacter>(InPawn);

	if (Bot)
	{
		//BlackboardComp->SetValueAsObject(FName("PatrolPoint"), Bot->GetNearestPatrolPoint());// Set the bla to the actual value;
	}
}
