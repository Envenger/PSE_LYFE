// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimInstance.h"
#include  "Player/Character/PSE_LYFE_Character5_Cover.h"
#include "PSE_LYFE_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API UPSE_LYFE_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeInitializeAnimation() override;

public:

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = Cover)
	ECoverState CoverState;
	
	UPROPERTY(VisibleDefaultsOnly, BluePrintReadOnly, Category = Weapon)
	FVector HandAttachmentLocation;
	
/*	const FName GetCurrentState(int32 MachineIndex = 0);*/

	
};
