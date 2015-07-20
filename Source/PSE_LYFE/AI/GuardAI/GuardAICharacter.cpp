// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "GuardAICharacter.h"


// Sets default values
AGuardAICharacter::AGuardAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bChoseRandomPatrolPoints = false;

	MinPatrolWaitTime = 3;
	MaxPatrolWaitTime = 6;
}

// Called when the game starts or when spawned
void AGuardAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGuardAICharacter::GetNextPatrolPoint(const uint8 CurrentPatrolPointIndex, uint8& NewPatrolPointIndex, AAITargetPoint* NewPatrolPoint) const
{
	//return AIGuardPoints[0];
}

void AGuardAICharacter::GetNearestPatrolPoint(uint8& NewPatrolPointIndex, AAITargetPoint* NewPatrolPoint) const
{
	AAITargetPoint* NearestpatrolPoint = nullptr;
	uint8 i;
	for (i = 0; i < AIGuardPoints.Num(); i++)
	{
		AAITargetPoint* PatrolPoint = AIGuardPoints[i];
		if (NearestpatrolPoint)
		{
			const float NearestPatrolPointDis = FVector::Dist(PatrolPoint->GetActorLocation(), GetActorLocation());
			const float CurrentPatrolPointDis = FVector::Dist(PatrolPoint->GetActorLocation(), GetActorLocation());
			if (CurrentPatrolPointDis < NearestPatrolPointDis)
			{
				NearestpatrolPoint = PatrolPoint;
			}
		}
		else
		{
			NearestpatrolPoint = PatrolPoint;
		}
	}
	NewPatrolPoint = NearestpatrolPoint;
	NewPatrolPointIndex = i;
}