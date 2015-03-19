// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_Item_Base_Stackable.h"

APSE_LYFE_Item_Base_Stackable::APSE_LYFE_Item_Base_Stackable(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bStackable = true;
	MaxStacks = 10;
}

FStackableItemStruct APSE_LYFE_Item_Base_Stackable::CreateStruct()
{
	FStackableItemStruct TempStruct(GetClass(), ItemType, ItemIcon, bStackable, MaxStacks);
	return TempStruct;
}


