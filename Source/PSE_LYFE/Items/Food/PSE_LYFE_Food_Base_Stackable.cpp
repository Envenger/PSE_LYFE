// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_Food_Base_Stackable.h"


APSE_LYFE_Food_Base_Stackable::APSE_LYFE_Food_Base_Stackable(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	ItemType = EItemType::StackableSingleConsumable;
	MaxStacks = 5;
	HungerRestore = 0;
	ThirstRestore = 0;
}

FStackableSingleFoodItemStruct APSE_LYFE_Food_Base_Stackable::CreateStruct()
{
	FStackableSingleFoodItemStruct TempStruct(ItemClass, ItemType, ItemIcon, bStackable, MaxStacks, HungerRestore, ThirstRestore);
	return TempStruct;
}
