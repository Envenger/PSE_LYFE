// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_Item_Base.h"
#include "PSE_LYFE_Item_Base_Stackable.generated.h"

USTRUCT()
struct FStackableItemStruct : public FItemStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	int32 CurrentStacks;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	int32 MaxStacks;

	virtual void CreateItem(UWorld* World, FVector SpawnLocation = FVector(0, 0, 0), FRotator SpawnRotation = FRotator(0, 0, 0)) override
	{
	}

	FStackableItemStruct(TSubclassOf<class APSE_LYFE_Item_Base> NewItemClass, TEnumAsByte<EItemType::Type> NewItemType, FSlateBrush NewItemIcon, bool bIsStackable,
		int32 MaximumStack)
	{
		ItemClass = NewItemClass;
		ItemType = NewItemType;
		ItemIcon = NewItemIcon;
		bStackable = bIsStackable;
		MaxStacks = MaximumStack;
		CurrentStacks = 1;
	}

	FStackableItemStruct()
	{
	}

};

UCLASS()
class PSE_LYFE_API APSE_LYFE_Item_Base_Stackable : public APSE_LYFE_Item_Base
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_Item_Base_Stackable(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties)
	int32 MaxStacks;

	FStackableItemStruct CreateStruct();
	
	
};
