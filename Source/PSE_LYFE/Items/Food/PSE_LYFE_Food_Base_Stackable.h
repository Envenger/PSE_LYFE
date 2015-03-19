// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Items/PSE_LYFE_Item_Base_Stackable.h"
#include "PSE_LYFE_Food_Base_Stackable.generated.h"

USTRUCT()
struct FStackableSingleFoodItemStruct : public FStackableItemStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	int32 HungerRestore;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
		int32 ThirstRestore;

	virtual void CreateItem(UWorld* World, FVector SpawnLocation = FVector(0, 0, 0), FRotator SpawnRotation = FRotator(0, 0, 0)) override
	{
		World->SpawnActor(ItemClass, &SpawnLocation, &SpawnRotation);
	}

	FStackableSingleFoodItemStruct(TSubclassOf<class APSE_LYFE_Item_Base> NewItemClass, TEnumAsByte<EItemType::Type> NewItemType, FSlateBrush NewItemIcon, bool bIsStackable,
		int32 MaximumStack, int32 Hunger, int32 Thirst)
	{
		ItemClass = NewItemClass;
		ItemType = NewItemType;
		ItemIcon = NewItemIcon;
		bStackable = bIsStackable;
		MaxStacks = MaximumStack;
		CurrentStacks = 1;
		HungerRestore = Hunger;
		ThirstRestore = Thirst;
	}

	FStackableSingleFoodItemStruct()
	{
		CurrentStacks = 1;
		MaxStacks = 5;
	}
};
UCLASS()
class PSE_LYFE_API APSE_LYFE_Food_Base_Stackable : public APSE_LYFE_Item_Base_Stackable
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_Food_Base_Stackable(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties)
	int32 HungerRestore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties)
	int32 ThirstRestore;

	FStackableSingleFoodItemStruct CreateStruct();
	
	
};
