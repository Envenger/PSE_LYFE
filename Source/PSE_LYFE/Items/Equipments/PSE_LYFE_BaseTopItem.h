// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_BaseTopItem.generated.h"

USTRUCT()
struct FCharacterTopComponent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Top)
	USkeletalMesh* TopMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Top)
	UMaterial* TopMaterial;

	bool IsValidComponent()
	{
		if (TopMesh && TopMaterial)
		{
			return true;
		}
		return false;
	}

	FCharacterTopComponent()
	{}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseTopItem : public APSE_LYFE_BaseInventoryItem
{
	GENERATED_BODY()

public:

	APSE_LYFE_BaseTopItem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Top)
	FCharacterTopComponent TopStruct;
	
	
};
