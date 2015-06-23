// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character5_Cover.h"
#include "PSE_LYFE_CMovementComponent.h"

float UPSE_LYFE_CMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();
	APSE_LYFE_Character4_Weapon* OwnerCharacter = Cast<APSE_LYFE_Character4_Weapon>(PawnOwner);
	if (OwnerCharacter)
	{
		if (OwnerCharacter->CrouchState != ECrouchState::Null)
		{
			return FMath::Lerp(MaxSpeed, MaxWalkSpeedCrouched, OwnerCharacter->AnimBP_CrouchStandAlpha);
		}
		else if (OwnerCharacter->bIsSprinting == true)
		{
			return MaxWalkSpeedSprint;
		}
	}

	return MaxSpeed;
}