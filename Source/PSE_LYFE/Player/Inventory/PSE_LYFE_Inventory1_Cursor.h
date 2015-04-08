// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Inventory/PSE_LYFE_Inventory0_Base.h"
#include "PSE_LYFE_InventoryStructures.h"
#include "PSE_LYFE_Inventory1_Cursor.generated.h"



UCLASS()
class PSE_LYFE_API APSE_LYFE_Inventory1_Cursor : public APSE_LYFE_Inventory0_Base
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	APSE_LYFE_Inventory1_Cursor();
	
	bool bIsCursorFunctional;

	/** Alt pressed or not. Used for alt left click */
	bool bIsAltPressed;

	UPROPERTY(replicated, Repretry)
	FItemStruct CursorItem;

	void AddItemToCursor(FItemStruct& StoredItem, FStorageLoc StoredLoc);

	void AddItemToCursorAlt(FItemStruct& StoredItem, FStorageLoc StoredLoc);

	void CursorSwap(FItemStruct& StoredItem, FStorageLoc StoredLoc);

	void ItemPicked(AActor* StoragePtr, FStorageLoc StoredLoc);

	//void ItemPicked(AInventoryStorage* StoragePtr, EquipmentLoc uint8);

	/** Resets cursor item location to its last stored location */
	void ResetItemLastLocation();

	UFUNCTION(reliable, server, WithValidation)
	void Server_ResetItemLastLocation();
	bool Server_ResetItemLastLocation_Validate();
	void Server_ResetItemLastLocation_Implementation();

	/** Actor is used cause item slot array can varry from 1d to 2d arrays */
	UPROPERTY()
	AActor* LastStoragePtr;

	/** Last location item was stored before the item was picked up */
	FStorageLoc LastStoredLoc;

	/** Lost stored location incase item was from an equipment slot */
	uint8 LastStoredEquipmentLoc;

	void AltPressed();

	UFUNCTION(reliable, server, WithValidation)
	void Server_AltPressed();
	bool Server_AltPressed_Validate();
	void Server_AltPressed_Implementation();

	void AltReleased();

	UFUNCTION(reliable, server, WithValidation)
	void Server_AltReleased();
	bool Server_AltReleased_Validate();
	void Server_AltReleased_Implementation();

/////////////////////////////////////////////////////////////
// Throw Item

	void ThrowCursorItem();

	UFUNCTION(reliable, server, WithValidation)
	void Server_ThrowCursorItem();
	bool Server_ThrowCursorItem_Validate();
	void Server_ThrowCursorItem_Implementation();

	void ThrowAllCursorItems();
	

	
};
