// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Inventory/PSE_LYFE_Inventory3_Equipment.h"
#include "PSE_LYFE_Inventory4_QuickSlots.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_Inventory4_QuickSlots : public APSE_LYFE_Inventory3_Equipment
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_Inventory4_QuickSlots();

	virtual void PostInitializeComponents() override;

	////////////////////////////////////////////////
	// QuickSlot

	uint8 NoOfQuickSlots;

	TArray<TSubclassOf<class APSE_LYFE_BaseInventoryItem>> QuickSlots;

	void InitializeQuickSlots();

	void QuickSlotUseItem(const uint8 SlotLoc);

	/////////////////////////////////////////////////
	//

	const FStorageLoc FindFirstItemOfType(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass);

	const uint32 FindTotalItemsOfType(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass);

	/////////////////////////////////////////////////
	// Keys

	void QuickSlotLeftClick(const uint8 SlotLoc);

	UFUNCTION(reliable, server, WithValidation)
	void Server_QuickSlotLeftClick(const uint8 SlotLoc);
	bool Server_QuickSlotLeftClick_Validate(const uint8 SlotLoc);
	void Server_QuickSlotLeftClick_Implementation(const uint8 SlotLoc);

	void QuickSlotRightClick(const uint8 SlotLoc);
	
	UFUNCTION(reliable, server, WithValidation)
	void Server_QuickSlotRightClick(const uint8 SlotLoc);
	bool Server_QuickSlotRightClick_Validate(const uint8 SlotLoc);
	void Server_QuickSlotRightClick_Implementation(const uint8 SlotLoc);
};
