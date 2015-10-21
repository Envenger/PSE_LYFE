// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "AI/GuardAI/GuardAICharacter.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AI/AITargetPoint.h"
#include "GuardAIController.h"

AGuardAIController::AGuardAIController()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComp"));
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));

	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PawnSensingComp"));

	AISight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISIghtSense"));
	AISight->DetectionByAffiliation.bDetectEnemies = true;
	AISight->DetectionByAffiliation.bDetectNeutrals = true;
	AISight->DetectionByAffiliation.bDetectFriendlies = true;

	PerceptionComp->ConfigureSense(*AISight);
	PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &AGuardAIController::SenseStuff);
}

void AGuardAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	BlackboardComp->InitializeBlackboard(*BotBehavior->BlackboardAsset);

	BehaviorComp->StartTree(*(BotBehavior));
	AGuardAICharacter* Bot = Cast<AGuardAICharacter>(InPawn);

	if (Bot)
	{
		BlackboardComp->SetValueAsObject(FName("SelfActor"), Bot);
	}
}

void AGuardAIController::SenseStuff(TArray<AActor*> testActors)
{

}