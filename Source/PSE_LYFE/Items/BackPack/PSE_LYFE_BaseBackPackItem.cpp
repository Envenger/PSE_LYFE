// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_BaseBackPackItem.h"


APSE_LYFE_BaseBackPackItem::APSE_LYFE_BaseBackPackItem()
{
	ItemType = EItemType::EquipableItem;
	EquipmentSlotType = EEquipmentSlotType::Backpack;
	BackpackSize = 0;
}

