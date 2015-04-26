// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_BaseWeaponMagazine.h"


APSE_LYFE_BaseWeaponMagazine::APSE_LYFE_BaseWeaponMagazine()
{
	ItemType = EItemType::StackableItem;
	MaxStacks = 100;

	MaxUses = 1;
	CurrentUses = MaxUses;

	EquipmentSlotType = EEquipmentSlotType::Nothing;
}

