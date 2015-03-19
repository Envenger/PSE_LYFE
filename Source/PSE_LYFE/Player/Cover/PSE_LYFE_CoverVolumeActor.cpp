// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_CoverVolumeActor.h"

APSE_LYFE_CoverVolumeActor::APSE_LYFE_CoverVolumeActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TriggerVolume = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Volume"));
	RootComponent = TriggerVolume;
}

