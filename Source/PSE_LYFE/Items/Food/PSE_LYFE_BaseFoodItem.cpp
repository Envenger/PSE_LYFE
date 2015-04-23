// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include"Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "PSE_LYFE_BaseFoodItem.h"

APSE_LYFE_BaseFoodItem::APSE_LYFE_BaseFoodItem()
{
	HungerRestore = 20;
	ThirstRestore = 0;
}

void APSE_LYFE_BaseFoodItem::UseItem(APSE_LYFE_Character4_Weapon* UsingPawn) const
{
	Super::UseItem(UsingPawn);
	if (HungerRestore != 0)
	{
		float PawnHunger = UsingPawn->GetCurrentHunger();
		PawnHunger = FMath::Max((PawnHunger - HungerRestore), 0.0f);
		UsingPawn->SetCurrentHunger(PawnHunger);
	}
}


