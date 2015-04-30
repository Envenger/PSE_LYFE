// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_BaseBottomItem.generated.h"

USTRUCT()
struct FCharacterBottomComponent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Bottom)
	USkeletalMesh* BottomMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Bottom)
	UMaterial* BottomMaterial;

	bool bIsBodyIncluded;

	bool IsValidComponent()
	{
		if (BottomMesh && BottomMaterial)
		{
			return true;
		}
		return false;
	}

	FCharacterBottomComponent()
	{
		bIsBodyIncluded = true;
	}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseBottomItem : public APSE_LYFE_BaseInventoryItem
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_BaseBottomItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Bottom)
	FCharacterBottomComponent BottomStruct;

};
