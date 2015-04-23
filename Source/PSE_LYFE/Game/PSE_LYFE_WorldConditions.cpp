// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_WorldConditions.h"


// Sets default values
APSE_LYFE_WorldConditions::APSE_LYFE_WorldConditions()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HungerDeducationPerTick = 60;

	HungerDeducationPerMinuite = 0.5;
}

// Called when the game starts or when spawned
void APSE_LYFE_WorldConditions::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APSE_LYFE_WorldConditions::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

