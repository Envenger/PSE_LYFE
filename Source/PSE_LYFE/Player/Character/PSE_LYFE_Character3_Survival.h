// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Character/PSE_LYFE_Character2_Inventory.h"
#include "PSE_LYFE_Character3_Survival.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_Character3_Survival : public APSE_LYFE_Character2_Inventory
{
	GENERATED_BODY()

public:

	APSE_LYFE_Character3_Survival();
	
	virtual void Tick(float DeltaSeconds) override;

//////////////////////////////////////////////////////////////////////////
// Character Stats

	/** Current Stamina that is replicated */
	UPROPERTY(Replicated)
	int8 CurrentStaminaRep;

	/** This holds the actual value of stamina it is decreased to int8 just for replication purposes */
	float CurrentStamina;

	/** Current Stamina */
	int8 MaxStamina;

	/** Stamina increase per second */
	float StaminaRegenRate;
};
