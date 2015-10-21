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

	GuardAIState = EGuardAIState::Null;

	AnimBP_bHasEquipedWeapon = false;
}

// Called when the game starts or when spawned
void AGuardAICharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AGuardAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGuardAICharacter::GetNextPatrolPoint(const uint8 CurrentPatrolPointIndex, uint8& NewPatrolPointIndex, AAITargetPoint*& NewPatrolPoint) const
{
	NewPatrolPointIndex = CurrentPatrolPointIndex + 1;
	if (NewPatrolPointIndex >= AIGuardPoints.Num())
	{
		NewPatrolPointIndex = 0;
	}
	NewPatrolPoint = AIGuardPoints[NewPatrolPointIndex];
}

void AGuardAICharacter::GetClosestPatrolPoint(uint8& NewPatrolPointIndex, AAITargetPoint*& NewPatrolPoint) const
{
	AAITargetPoint* NearestPatrolPoint = nullptr;
	uint8 NearestPatrolPointIndex = 0;
	for (uint8 i = 0; i < AIGuardPoints.Num(); i++)
	{
		AAITargetPoint* PatrolPoint = AIGuardPoints[i];
		if (NearestPatrolPoint)
		{
			const float NearestPatrolPointDis = FVector::Dist(PatrolPoint->GetActorLocation(), GetActorLocation());
			const float CurrentPatrolPointDis = FVector::Dist(PatrolPoint->GetActorLocation(), GetActorLocation());
			if (CurrentPatrolPointDis < NearestPatrolPointDis)
			{
				NearestPatrolPoint = PatrolPoint;
				NearestPatrolPointIndex = i;
			}
		}
		else
		{
			NearestPatrolPoint = PatrolPoint;
			NearestPatrolPointIndex = i;
		}
	}
	NewPatrolPointIndex = NearestPatrolPointIndex;
	NewPatrolPoint = NearestPatrolPoint;
}

bool AGuardAICharacter::SetAIGuardState(EGuardAIState NewAIState)
{
	if (NewAIState != GuardAIState)
	{
		if (NewAIState == EGuardAIState::Patroling)
		{
			GetCharacterMovement()->MaxWalkSpeed = 300;
			if (AnimBP_bHasEquipedWeapon)
			{
				UnEquipWeapon();
			}
			GuardAIState = NewAIState;
		}
		else if (NewAIState == EGuardAIState::Pursuing)
		{
			if (!AnimBP_bHasEquipedWeapon)
			{
				EquipWeapon();
			}
			GetCharacterMovement()->MaxWalkSpeed = 600;
		}
	}
	return true;
}

void AGuardAICharacter::EquipWeapon()
{
	AnimBP_bHasEquipedWeapon = true;
}

void AGuardAICharacter::UnEquipWeapon()
{
	AnimBP_bHasEquipedWeapon = false;
}

void AGuardAICharacter::AnimBP_EquipFinishNotify()
{

}

void AGuardAICharacter::AnimBP_UnEquipFinishNotify()
{

}