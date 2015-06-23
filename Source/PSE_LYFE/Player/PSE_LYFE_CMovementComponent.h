// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "PSE_LYFE_CMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API UPSE_LYFE_CMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = "Character Movement", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxWalkSpeedSprint;

	UPROPERTY(Category = "Character Movement", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float MaxWalkSpeedHighCover;

private:

	virtual float GetMaxSpeed() const override;

	
};
