// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_WorldConditions.h"
#include "PSE_LYFE_GameMode.h"

void APSE_LYFE_GameMode::BeginPlay()
{
	Super::BeginPlay();

	WorldCondtionActor = GetWorld()->SpawnActor<APSE_LYFE_WorldConditions>(WorldConditionClass);
}

void APSE_LYFE_GameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);

	// Can be used to spawn the inventory and link
}

