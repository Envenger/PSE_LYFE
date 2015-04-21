// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Character3_Survival.h"

APSE_LYFE_Character3_Survival::APSE_LYFE_Character3_Survival()
{
	StaminaRegenRate = 18;

	MaxStamina = 100;
	CurrentStamina = MaxStamina;
	CurrentStaminaRep = MaxStamina;
}

void APSE_LYFE_Character3_Survival::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentStamina < MaxStamina)
	{
		float StaminaIncrease = FMath::Min((MaxStamina - CurrentStamina), (StaminaRegenRate * DeltaSeconds));
		CurrentStamina += StaminaIncrease;
		CurrentStaminaRep = FMath::RoundToInt(CurrentStamina);
	}
}

void APSE_LYFE_Character3_Survival::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character3_Survival, CurrentStaminaRep, COND_OwnerOnly);

}