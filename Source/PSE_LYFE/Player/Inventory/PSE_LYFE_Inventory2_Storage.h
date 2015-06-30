// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Inventory/PSE_LYFE_Inventory1_Cursor.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Weapons/FiringWeapon/PSE_LYFE_ReloadableWeapon.h"
#include "PSE_LYFE_Inventory2_Storage.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_Inventory2_Storage : public APSE_LYFE_Inventory1_Cursor
{
	GENERATED_BODY()

public:
	
	APSE_LYFE_Inventory2_Storage();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	TSubclassOf<class APSE_LYFE_BaseInventoryItem> TestClass;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(ReplicatedUsing = OnRep_ClientResetBagSize)
	TArray<FItemStruct> BackPackBase;

	/** This stores the item structures pointers */
	UPROPERTY()
	FStorageArray BackPack;

//////////////////////////////////////////////////

	/** Default Storage size without any bags */
	uint16 DefaultBagPackSize;

	/** Row, Col size of the storage */
	uint16 BackPackSize;

	const uint16 GetTotalBackPackSize() const;

	void SetBackPackSize();

	void TestFunction();

	void ResetBackPackSize(const int16 NewBagSize);

	const int16 GetLowestItemIndex() const;

	UFUNCTION()
	void OnRep_ClientResetBagSize();

	FStorageLoc BackPackDisplaySize;

	/** Finds the nearest empty item slot returns -1, -1 if now slot is empty */
	const FStorageLoc FindFirstEmptySlot();

	const FStorageLoc FindFirstFreeSlot(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass);

	/** Deletes item from the storage. Stack < 0 means delete all items of that slot */
	void DeleteItems(const FStorageLoc ItemLoc, const int32 Stacks = -1);

	/** Adds StackableItem if inventory location is not given, it find the first nonempty slot */
	const bool AddItem(FItemStruct &ItemStruct, const FStorageLoc ItemLoc = FStorageLoc(-1, -1));
	
/////////////////////////////////////////
// Clicks //

	void BackPackSlotLeftClick(const FStorageLoc ItemLoc);

	UFUNCTION(reliable, server, WithValidation)
	void Server_BackPackSlotLeftClick(const FStorageLoc ItemLoc);
	bool Server_BackPackSlotLeftClick_Validate(const FStorageLoc ItemLoc);
	void Server_BackPackSlotLeftClick_Implementation(const FStorageLoc ItemLoc);
	
	void BackPackSlotRightClick(const FStorageLoc ItemLoc);

	UFUNCTION(reliable, server, WithValidation)
	void Server_BackPackSlotRightClick(const FStorageLoc ItemLoc);
	bool Server_BackPackSlotRightClick_Validate(const FStorageLoc ItemLoc);
	void Server_BackPackSlotRightClick_Implementation(const FStorageLoc ItemLoc);

	/** No server function needed as this is called form the right click */
	virtual void EquipItem(const FStorageLoc ItemLoc){};

//////////////////////////////////////////////
// On Item events //

	void ItemAdded(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks = 1);

	void ItemRemoved(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks = 1);

	void WeaponMagazineAdded(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass);

	void WeaponAdded(APSE_LYFE_ReloadableWeapon* ReloadableWeapon);


//////////////////////////////////////////////
// Total No Of Items //

	FTotalItemStruct TotalItemsArray;

	const float CalculateWeight(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks = 1) const;

	UPROPERTY(replicated)
	float TotalItemsWeight;

	/** Only Present on client side calculated via the on rep function */
	uint16 NoOfSlotsUsed;

	const float GetNoOfUsedSlots() const;

	void CloseInventory();

	uint16 Int1;
	uint16 Int2;
	
};
