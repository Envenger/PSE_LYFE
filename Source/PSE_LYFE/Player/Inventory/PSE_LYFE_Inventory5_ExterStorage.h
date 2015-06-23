// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "Storage/PSE_LYFE_BaseStorage.h"
#include "PSE_LYFE_Inventory5_ExterStorage.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_Inventory5_ExterStorage : public APSE_LYFE_Inventory4_QuickSlots
{
	GENERATED_BODY()

public:

	APSE_LYFE_Inventory5_ExterStorage();
	
	UPROPERTY(ReplicatedUsing=OnRep_StoragePointerChange)
	APSE_LYFE_BaseStorage* OpenedStorage;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void OnRep_StoragePointerChange();

	void CloseStorage();

	UFUNCTION(reliable, server, WithValidation)
	void Server_CloseStorage();
	bool Server_CloseStorage_Validate();
	void Server_CloseStorage_Implementation();

	void StorageSlotLeftClick(const FStorageLoc ItemLoc);

	UFUNCTION(reliable, server, WithValidation)
	void Server_StorageSlotLeftClick(const FStorageLoc ItemLoc);
	bool Server_StorageSlotLeftClick_Validate(const FStorageLoc ItemLoc);
	void Server_StorageSlotLeftClick_Implementation(const FStorageLoc ItemLoc);

	void StorageSlotRightClick(const FStorageLoc ItemLoc);

	UFUNCTION(reliable, server, WithValidation)
	void Server_StorageSlotRightClick(const FStorageLoc ItemLoc);
	bool Server_StorageSlotRightClick_Validate(const FStorageLoc ItemLoc);
	void Server_StorageSlotRightClick_Implementation(const FStorageLoc ItemLoc);

	const FStorageLoc FindFirstEmptySlotStorage();

	const FStorageLoc FindFirstFreeSlotStorage(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass);

	void DeleteItemsStorage(const FStorageLoc ItemLoc, const int32 Stacks = -1);

	const bool AddItemStorage(FItemStruct &ItemStruct, const FStorageLoc ItemLoc = FStorageLoc(-1, -1));
};
