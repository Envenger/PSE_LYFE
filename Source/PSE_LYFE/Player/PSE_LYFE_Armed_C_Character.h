// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/PSE_LYFE_ArmedCharacter.h"
#include "PSE_LYFE_Armed_C_Character.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_Armed_C_Character : public APSE_LYFE_ArmedCharacter
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* HighTraceLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* LowTraceLoc;

public:

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<AActor> ActorClass;

	AActor* one;

	AActor* two;

	APSE_LYFE_Armed_C_Character(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

///////////////////////////////////////////////////////////////////
//////////// Cover Mechanics

	UPROPERTY(Replicated)
	uint8 CoveredCounter;

	/** Actor Overlap start */
	virtual void ReceiveActorBeginOverlap(AActor* OtherActor) override;

	/** Actor Overlap end */
	virtual void ReceiveActorEndOverlap(AActor* OtherActor) override;

///////////////////////////////////////////////////////////////////
	
	
};
