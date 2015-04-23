// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_BaseFoodItem.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseFoodItem : public APSE_LYFE_BaseInventoryItem
{
	GENERATED_BODY()

public:

	APSE_LYFE_BaseFoodItem();

	virtual void UseItem(APSE_LYFE_Character4_Weapon* UsingPawn) const;

private:
	
	UPROPERTY(EditdefaultsOnly, Category = FoodItem)
	float HungerRestore;

	UPROPERTY(EditdefaultsOnly, Category = FoodItem)
	float ThirstRestore;
};
