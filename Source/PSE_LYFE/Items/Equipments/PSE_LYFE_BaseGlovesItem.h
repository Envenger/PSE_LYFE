// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_BaseGlovesItem.generated.h"

USTRUCT()
struct FCharacterGlovesComponent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gloves)
	UMaterial* GloveMaterial;

	bool IsValidComponent()
	{
		if (GloveMaterial)
		{
			return true;
		}
		return false;
	}

	FCharacterGlovesComponent()
	{}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseGlovesItem : public APSE_LYFE_BaseInventoryItem
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_BaseGlovesItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gloves)
	FCharacterGlovesComponent GlovesStruct;

	
};
