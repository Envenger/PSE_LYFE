// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Storage/PSE_LYFE_InventoryStructures.h"
#include "PSE_LYFE_PlayerInventory.generated.h"

class APSE_LYFE_Character;

UCLASS()
class PSE_LYFE_API APSE_LYFE_PlayerInventory : public AActor
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_PlayerInventory(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(Replicated)
	APSE_LYFE_Character* InventoryOwner;

	void SetOwningPawn(APSE_LYFE_Character* Owner);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = BaseItem)
	UStaticMeshComponent* StorageMesh;

	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

	/** This stores only the the reference to the actual storage of the items stored in the Main arrays */
	UPROPERTY(Replicated)
	FStorageArray Storage;

	bool bIsCursorFunctional;

///////////////////////////////////////////////////
/// Actual Arrays

	UPROPERTY(Replicated)
	TArray<FStackableSingleFoodItemLoc> StackableSingleFoodItemArray;

//////////////////////////////////////////////////

	/** Row, Col size of the storage */
	FStorageLoc StorageSize;

	void SetBagSize(FStorageLoc BagSize);

	/** Finds the nearest empty item slot returns -1, -1 if now slot is empty */
	FStorageLoc FindFirstEmptySlot();

	int32 FindFirstFreeSlot(TSubclassOf<class APSE_LYFE_Item_Base> ItemClass);

	/** Uses the item as needed */
	void UseItem(FStorageLoc ItemLoc);

	/** Deletes item from the storage. Stack < 0 means delete all items of that slot */
	void DeleteItems(FStorageLoc ItemLoc, int32 Stacks = 1);

	/** Called to update the HUD incase it is open(will be used only if tick updating is not used) */
	void CreateItemCursor();

	/** Adds StackableSingleFoodItem if inventory locaiton is not given, it find the first nonempty slot */
	void AddItem(FStackableSingleFoodItemStruct &StackableSingleFoodItem, FStorageLoc ItemLoc = FStorageLoc(-1, -1));


///////////////////////////////////////////////
///////// Board Design

	/** Design of the board */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush InventoryBorder;

	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush NothingCursor;

	/** Icon of an emptry slot */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush EmptySlotImg;

	/** Font Background Image */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateFontInfo StacksFont;

/////////////////////////////////////////
// Cursor Values //

	UPROPERTY(Replicated)
	TEnumAsByte<EItemType::Type> CursorItemType;

	UPROPERTY(Replicated)
	FStackableSingleFoodItemStruct StackableSingleFoodItemCursor;

	void UpdateCursorItem();

	/** Adds the item to the cursor */
	void AddItemToCursor(FStorageLoc ItemLocn);

	/** Last location item was stored before the item was picked up */
	FStorageLoc LastStoredLoc;

	/** Swaps the item between cursor and the clicked position */
	void CursorSwap(FStorageLoc ItemLoc);

	void ThrowAllCursorItems();

	void ThrowCursorItem();

/////////////////////////////////////////
// Cursor Click //

	void AttemptPickItem(APSE_LYFE_Item_Base* PickedItem);

	void LeftClick(FStorageLoc ItemLoc);

/////////////////////////////////////////

};
