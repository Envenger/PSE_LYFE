// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_WorldConditions.generated.h"

UCLASS()
class PSE_LYFE_API APSE_LYFE_WorldConditions : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSE_LYFE_WorldConditions();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Tick time for Hunger and Thirst in real time(seconds) */
	UPROPERTY(EditDefaultsOnly, Category = WorldConditions)
	float HungerIncreasePerTick;

	/** Value of hunger reduced pet minute of real time */
	UPROPERTY(EditDefaultsOnly, Category = WorldConditions)
	float TickDuration;
	
};
