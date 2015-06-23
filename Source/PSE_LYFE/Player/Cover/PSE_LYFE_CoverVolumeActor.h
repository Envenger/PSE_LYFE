// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_CoverVolumeActor.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_CoverVolumeActor : public AActor
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_CoverVolumeActor(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UBoxComponent* TriggerVolume;

	/** Actor Overlap start */
	virtual void ReceiveActorBeginOverlap(AActor* OtherActor) override;

	/** Actor Overlap end */
	virtual void ReceiveActorEndOverlap(AActor* OtherActor) override;
	
};
