// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_BaseBackPackItem.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseBackPackItem : public APSE_LYFE_BaseInventoryItem
{
	GENERATED_BODY()

public:

	APSE_LYFE_BaseBackPackItem();

	UPROPERTY(EditDefaultsOnly, Category = BagSize)
	uint8 BackpackSize;
};
