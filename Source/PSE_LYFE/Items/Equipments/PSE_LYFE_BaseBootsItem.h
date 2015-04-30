// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_BaseBootsItem.generated.h"

USTRUCT()
struct FCharacterBootsComponent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Boots)
	USkeletalMesh* BootsMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Boots)
	UMaterial* BootsMaterial;

	bool IsValidComponent()
	{
		if (BootsMesh && BootsMaterial)
		{
			return true;
		}
		return false;
	}

	FCharacterBootsComponent()
	{}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseBootsItem : public APSE_LYFE_BaseInventoryItem
{
	GENERATED_BODY()

public:

	APSE_LYFE_BaseBootsItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Boots)
	FCharacterBootsComponent BootsStruct;
	
	
};
