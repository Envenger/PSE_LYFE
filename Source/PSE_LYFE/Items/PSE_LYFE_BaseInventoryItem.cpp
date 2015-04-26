// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_BaseInventoryItem.h"


// Sets default values
APSE_LYFE_BaseInventoryItem::APSE_LYFE_BaseInventoryItem()
{
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;
	

	ItemType = EItemType::BaseItem;
	MaxStacks = 1;

	MaxUses = 1;
	CurrentUses = MaxUses;

	EquipmentSlotType = EEquipmentSlotType::Nothing;


	bReplicates = true;
	bReplicateMovement = true;
}

FItemStruct APSE_LYFE_BaseInventoryItem::CreateStruct() const
{
	FItemStruct TempStruct(GetClass());
	if (ItemType == EItemType::StackableItem || ItemType == EItemType::StackableUsableItem)
	{
		TempStruct.ItemProperties.Add(1);//First slot gives the number of stacks
	}
	else if (ItemType == EItemType::MultiUseItem)
	{
		TempStruct.ItemProperties.Add(CurrentUses);
	}
	return TempStruct;
}

void APSE_LYFE_BaseInventoryItem::UseItem(APSE_LYFE_Character4_Weapon* UsingPawn) const
{

}

void APSE_LYFE_BaseInventoryItem::EquipItem() const
{

}

