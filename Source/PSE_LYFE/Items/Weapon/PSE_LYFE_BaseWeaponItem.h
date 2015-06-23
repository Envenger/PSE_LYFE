// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Weapons/BaseFiles/PSE_LYFE_BaseWeapon.h"
#include "PSE_LYFE_BaseWeaponItem.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseWeaponItem : public APSE_LYFE_BaseInventoryItem
{
	GENERATED_BODY()

public:

	APSE_LYFE_BaseWeaponItem();
	
	UPROPERTY(EditDefaultsOnly, Category = ItemProperties)
	TSubclassOf<class APSE_LYFE_BaseWeapon> PlayerWeaponClass;
};
