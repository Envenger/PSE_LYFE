// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "PSE_LYFE_GameMode.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_GameMode : public AGameMode
{
	GENERATED_BODY()

	void RestartPlayer(AController* NewPlayer) override;
	
	
};
