// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Game/PSE_LYFE_GameMode.h"
#include "Game/PSE_LYFE_WorldConditions.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Character3_Survival.h"

APSE_LYFE_Character3_Survival::APSE_LYFE_Character3_Survival()
{
	StaminaRegenRate = 18;

	MaxStamina = 100;
	CurrentStamina = MaxStamina;
	CurrentStaminaRep = MaxStamina;

	CurrentHunger = 0;
	CurrentHungerRep = 5;// CurrentHunger;

	HungerReductionPercentage = 0;
}

void APSE_LYFE_Character3_Survival::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority)
	{
		AGameMode* GameMode = GetWorld()->GetAuthGameMode();
		if (GameMode && GameMode->IsA(APSE_LYFE_GameMode::StaticClass()))
		{
			APSE_LYFE_GameMode* PSEGameMode = Cast<APSE_LYFE_GameMode>(GameMode);
			WorldCondtionActor = PSEGameMode->WorldCondtionActor;
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "WOrld condition works");

			//FTimerHandle HungerTimerHandle;
			//GetWorldTimerManager().SetTimer(HungerTimerHandle, this, &APSE_LYFE_Character3_Survival::CalculateHunger, WorldCondtionActor->TickDuration, true);
		}

	}
}

void APSE_LYFE_Character3_Survival::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Role == ROLE_Authority)
	{
		if (CurrentStamina < MaxStamina)
		{
			float StaminaIncrease = FMath::Min((MaxStamina - CurrentStamina), (StaminaRegenRate * DeltaSeconds));
			CurrentStamina += StaminaIncrease;
			CurrentStaminaRep = FMath::RoundToInt(CurrentStamina);
		}
	}
}

const int8 APSE_LYFE_Character3_Survival::GetCurrentStaminaDisplay() const
{
	return CurrentStaminaRep;
}

void APSE_LYFE_Character3_Survival::CalculateHunger()
{
	CurrentHunger = FMath::Max(MaxHunger, (CurrentHunger + GetHungerIncrease()));
	CurrentHungerRep = FMath::RoundToInt(CurrentHunger);
}

const float APSE_LYFE_Character3_Survival::GetCurrentHunger() const
{
	return CurrentHunger;
}

const float APSE_LYFE_Character3_Survival::GetMaxHunger() const
{
	return MaxHunger;
}

void APSE_LYFE_Character3_Survival::SetCurrentHunger(float NewHunger)
{
	CurrentHunger = NewHunger;
	CurrentHungerRep = FMath::RoundToInt(CurrentHunger);
}

const float APSE_LYFE_Character3_Survival::GetHungerIncrease()
{
	if (WorldCondtionActor)
	{
		float HungetDeducation = WorldCondtionActor->HungerIncreasePerTick;
		HungetDeducation = HungetDeducation* ((100 - HungetDeducation) / 100);
		return HungetDeducation;
	}
	return 0;
}

void APSE_LYFE_Character3_Survival::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character3_Survival, CurrentStaminaRep, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character3_Survival, CurrentHungerRep, COND_OwnerOnly);
}