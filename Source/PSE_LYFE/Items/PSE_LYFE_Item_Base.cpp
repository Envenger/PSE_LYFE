// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_Item_Base.h"

APSE_LYFE_Item_Base::APSE_LYFE_Item_Base(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	ItemType = EItemType::Nothing;
	bStackable = false;
	ItemClass = GetClass();

	bReplicates = true;
	bReplicateMovement = true;
}

FItemStruct APSE_LYFE_Item_Base::CreateStruct()
{
	FItemStruct TempStruct(GetClass(), ItemType, ItemIcon, bStackable);
	return TempStruct;
}



