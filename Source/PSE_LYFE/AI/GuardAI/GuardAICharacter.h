// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AI/AITargetPoint.h"
#include "GuardAICharacter.generated.h"



UCLASS()
class PSE_LYFE_API AGuardAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGuardAICharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//UPROPERTY(EditAnywhere, Category = Behavior)
	//class UBehaviorTree* BotBehavior;
	
	UPROPERTY(EditInstanceOnly, Category = Patroling)
	TArray<AAITargetPoint*> AIGuardPoints;

	UPROPERTY(EditInstanceOnly, Category = Patroling)
	bool bChoseRandomPatrolPoints;

	UPROPERTY(EditInstanceOnly, Category = Patroling)
	float MinPatrolWaitTime;

	UPROPERTY(EditInstanceOnly, Category = Patroling)
	float MaxPatrolWaitTime;

	const AAITargetPoint* GetNextPatrolPoint() const;

	void GetNextPatrolPoint(const uint8 CurrentPatrolPointIndex, uint8& NewPatrolPointIndex, AAITargetPoint* NewPatrolPoint) const;

	void GetNearestPatrolPoint(uint8& NewPatrolPointIndex, AAITargetPoint* NewPatrolPoint) const;
};
