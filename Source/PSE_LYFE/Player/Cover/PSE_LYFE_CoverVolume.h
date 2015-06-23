// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Volume.h"
#include "PSE_LYFE_CoverVolume.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_CoverVolume : public AVolume
{
	GENERATED_BODY()
	
	APSE_LYFE_CoverVolume(const FObjectInitializer& ObjectInitializer);
	
	/** Actor Overlap start */
	virtual void ReceiveActorBeginOverlap(AActor* OtherActor) override;

	/** Actor Overlap end */
	virtual void ReceiveActorEndOverlap(AActor* OtherActor) override;
};
