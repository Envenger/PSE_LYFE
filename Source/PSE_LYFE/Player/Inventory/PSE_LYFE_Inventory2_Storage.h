// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Inventory/PSE_LYFE_Inventory1_Cursor.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
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

	UPROPERTY(ReplicatedUsing = OnRep_ClientResetBagSize, Repretry)
	TArray<FItemStruct> StorageBase;

	/** This stores the item structures pointers */
	UPROPERTY()
	FStorageArray Storage;

//////////////////////////////////////////////////

	/** Defualt Storage size without any bags */
	uint16 DefaultStorageSize;

	/** Row, Col size of the storage */
	uint16 BagSize;

	const uint16 GetTotalStorageSize() const;

	void SetStorageBagSize(const int16 BagSize);

	void TestFunction();

	void ResetStorageSize(const int16 NewBagSize);

	const int16 GetLowestItemIndex() const;

	UFUNCTION()
	void OnRep_ClientResetBagSize();

	FStorageLoc StorageDisplaySize;

	/** Used for changing position of 2 items in inventory but also can be used to move item(other is empty) */
	void SwapItems(const FStorageLoc Loc1, const FStorageLoc Loc2);

	/** Finds the nearest empty item slot returns -1, -1 if now slot is empty */
	const FStorageLoc FindFirstEmptySlot();

	const FStorageLoc FindFirstFreeSlot(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass);

	/** Deletes item from the storage. Stack < 0 means delete all items of that slot */
	void ThrowItems(const FStorageLoc ItemLoc, const int32 Stacks = -1);

	/** Deletes item from the storage. Stack < 0 means delete all items of that slot */
	void DeleteItems(const FStorageLoc ItemLoc, const int32 Stacks = -1);

	/** Adds StackableItem if inventory location is not given, it find the first nonempty slot */
	const bool AddItem(FItemStruct &ItemStruct, const FStorageLoc ItemLoc = FStorageLoc(-1, -1));
	
/////////////////////////////////////////
// Clicks //

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


	/** No server function needed as this is called form the right click */
	virtual void EquipItem(const FStorageLoc ItemLoc);

//////////////////////////////////////////////
// On Item events //

	void ItemAdded(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks = 1);

	void ItemRemoved(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks = 1);

//////////////////////////////////////////////
// Total No Of Items //

	FTotalItemStruct TotalItemsArray;

	uint16 Int1;
	uint16 Int2;
	
};
