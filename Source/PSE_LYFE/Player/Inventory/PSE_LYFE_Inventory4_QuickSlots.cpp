// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_Inventory4_QuickSlots.h"

APSE_LYFE_Inventory4_QuickSlots::APSE_LYFE_Inventory4_QuickSlots()
{
	NoOfQuickSlots = 5;
}

void APSE_LYFE_Inventory4_QuickSlots::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitializeQuickSlots();
}

void APSE_LYFE_Inventory4_QuickSlots::InitializeQuickSlots()
{
	TSubclassOf<class APSE_LYFE_BaseInventoryItem> TempClass = nullptr;
	for (uint8 i = 0; i < NoOfQuickSlots; i++)
	{
		QuickSlots.Add(TempClass);
	}
}

const FStorageLoc APSE_LYFE_Inventory4_QuickSlots::FindFirstItemOfType(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass)
{
	for (int32 i = 0; i < Storage.Rows.Num(); i++)
	{
		for (int32 j = 0; j < Storage.Rows[i].Columns.Num(); j++)
		{
			if (ItemClass == Storage.GetItem(FStorageLoc(i, j)).ItemClass)
			{
				return FStorageLoc(i, j);
			}
		}
	}
	return FStorageLoc(-1, -1);
}

const uint32 APSE_LYFE_Inventory4_QuickSlots::FindTotalItemsOfType(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass)
{
	uint32 NoOfStacks = 0;
	for (uint16 i = 0; i < StorageBase.Num(); i++)
	{
		if (ItemClass == StorageBase[i].ItemClass)
		{
			if (StorageBase[i].ItemProperties.IsValidIndex(0))
			{
				NoOfStacks += StorageBase[i].ItemProperties[0];
			}
			else
			{
				NoOfStacks += 1;
			}
		}
	}
	return(NoOfStacks);
}

void APSE_LYFE_Inventory4_QuickSlots::QuickSlotLeftClick(const uint8 SlotLoc)
{
	if (CursorItem.ItemClass != nullptr)
	{
		const APSE_LYFE_BaseInventoryItem* DefualtItem = CursorItem.GetDefaultItem();
		if (DefualtItem->ItemType == EItemType::UsableItem || DefualtItem->ItemType == EItemType::StackableUsableItem)
		{
			QuickSlots[SlotLoc] = CursorItem.ItemClass;
			ResetItemLastLocation();
		}
		{
			//Error message item cant be equiuped in quick slot
		}
	}
	else
	{
		QuickSlotUseItem(SlotLoc);
	}
}

void APSE_LYFE_Inventory4_QuickSlots::QuickSlotRightClick(const uint8 SlotLoc)
{
	if (QuickSlots[SlotLoc] != nullptr)
	{
		QuickSlots[SlotLoc] = nullptr;
	}
}

void APSE_LYFE_Inventory4_QuickSlots::QuickSlotUseItem(const uint8 SlotLoc)
{
	if (QuickSlots[SlotLoc] != nullptr)
	{
		const FStorageLoc ItemLoc = FindFirstItemOfType(QuickSlots[SlotLoc]);
		if (ItemLoc.RowNum > -1 && ItemLoc.ColNum > -1)
		{
			DeleteItems(ItemLoc, 1);
		}
	}
}
