// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/BaseFiles/PSE_LYFE_AutoRWeapon.h"
#include "PSE_LYFE_AutoRZWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_AutoRZWeapon : public APSE_LYFE_AutoRWeapon
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_AutoRZWeapon(const FObjectInitializer& ObjectInitializer);

	bool bZoomed;
	
	
};
