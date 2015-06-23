// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_InventoryStructures.h"
#include "PSE_LYFE_Inventory2_Storage.h"
#include "PSE_LYFE_Inventory3_Equipment.h"
#include "PSE_LYFE_Inventory5_ExterStorage.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Inventory1_Cursor.h"


// Sets default values
APSE_LYFE_Inventory1_Cursor::APSE_LYFE_Inventory1_Cursor()
{
	CursorItem.ItemClass = nullptr;

	bIsCursorFunctional = true;
	bIsAltPressed = false;
	LastStoragePtrType = EStorageType::Nothing;
}


void APSE_LYFE_Inventory1_Cursor::AddItemToCursor(FItemStruct& StoredItem, FStorageLoc StoredLoc, EStorageType StorageType)
{
	if (StorageType == EStorageType::BackPackSlot)
	{
		APSE_LYFE_Inventory2_Storage* BackPackPtr = Cast<APSE_LYFE_Inventory2_Storage>(this);
		CursorItem = StoredItem;
		BackPackPtr->DeleteItems(StoredLoc, -1); // All items deleted                                                                                                  
		ItemPicked(StoredLoc, EStorageType::BackPackSlot);
	}
	else if (StorageType == EStorageType::StorageSlot)
	{
		APSE_LYFE_Inventory5_ExterStorage* StoragePtr = Cast<APSE_LYFE_Inventory5_ExterStorage>(this);
		CursorItem = StoredItem;
		StoragePtr->DeleteItemsStorage(StoredLoc, -1); // All items deleted                                                                                                  
		ItemPicked(StoredLoc, EStorageType::StorageSlot);
	}
}

void APSE_LYFE_Inventory1_Cursor::AddItemToCursor(FItemStruct& StoredItem, uint8 EquipmentSlotLoc, EStorageType StorageType)
{
	APSE_LYFE_Inventory3_Equipment* EquipmentPtr = Cast<APSE_LYFE_Inventory3_Equipment>(this);
	CursorItem = StoredItem;
	EquipmentPtr->RemoveInventoryEquipment(EquipmentSlotLoc);
	ItemPicked(EquipmentSlotLoc);
}

void APSE_LYFE_Inventory1_Cursor::AddItemToCursorAlt(FItemStruct& StoredItem, FStorageLoc StoredLoc, EStorageType StorageType)
{
	if (StorageType == EStorageType::BackPackSlot)
	{
		APSE_LYFE_Inventory2_Storage* BackPackPtr = Cast<APSE_LYFE_Inventory2_Storage>(this);
		if (CursorItem.ItemClass == nullptr)
		{
			if (StoredItem.ItemClass != nullptr)
			{
				const APSE_LYFE_BaseInventoryItem* BaseItem = BackPackPtr->BackPack.GetItem(StoredLoc).GetDefaultItem();
				if (BaseItem->GetItemType() == EItemType::StackableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
				{
					int32 &CurrentStacks = StoredItem.ItemProperties[0];
					CursorItem = StoredItem;
					CursorItem.ItemProperties[0] = 1;
					BackPackPtr->DeleteItems(StoredLoc, 1); // Items deleted by 1 stack
					ItemPicked(StoredLoc, EStorageType::BackPackSlot);
				}
				else
				{
					AddItemToCursor(StoredItem, StoredLoc, EStorageType::BackPackSlot);
				}
			}
		}
		else if (StoredItem.ItemClass == CursorItem.ItemClass)
		{
			const APSE_LYFE_BaseInventoryItem* BaseItem = CursorItem.GetDefaultItem();
			const int32 MaxStacks = BaseItem->GetMaxStacks();
			int32 &CurrentStacks = CursorItem.ItemProperties[0];
			if (CurrentStacks < MaxStacks)
			{
				CurrentStacks += 1;
				BackPackPtr->DeleteItems(StoredLoc, 1); // Item delete by 1 stack
				ItemPicked(StoredLoc, EStorageType::BackPackSlot);
			}
		}
	}
	else if (StorageType == EStorageType::StorageSlot)
	{
		APSE_LYFE_Inventory5_ExterStorage* StoragePtr = Cast<APSE_LYFE_Inventory5_ExterStorage>(this);
		if (CursorItem.ItemClass == nullptr)
		{
			if (StoredItem.ItemClass != nullptr)
			{
				const APSE_LYFE_BaseInventoryItem* BaseItem = StoragePtr->OpenedStorage->Storage.GetItem(StoredLoc).GetDefaultItem();
				if (BaseItem->GetItemType() == EItemType::StackableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
				{
					int32 &CurrentStacks = StoredItem.ItemProperties[0];
					CursorItem = StoredItem;
					CursorItem.ItemProperties[0] = 1;
					StoragePtr->DeleteItemsStorage(StoredLoc, 1); // Items deleted by 1 stack
					ItemPicked(StoredLoc, EStorageType::BackPackSlot);
				}
				else
				{
					AddItemToCursor(StoredItem, StoredLoc, EStorageType::StorageSlot);
				}
			}
		}
		else if (StoredItem.ItemClass == CursorItem.ItemClass)
		{
			const APSE_LYFE_BaseInventoryItem* BaseItem = CursorItem.GetDefaultItem();
			const int32 MaxStacks = BaseItem->GetMaxStacks();
			int32 &CurrentStacks = CursorItem.ItemProperties[0];
			if (CurrentStacks < MaxStacks)
			{
				CurrentStacks += 1;
				StoragePtr->DeleteItemsStorage(StoredLoc, 1); // Item delete by 1 stack
				ItemPicked(StoredLoc, EStorageType::StorageSlot);
			}
		}
	}
}

void APSE_LYFE_Inventory1_Cursor::CursorSwap(FItemStruct& StoredItem, FStorageLoc StoredLoc, EStorageType StorageType)
{
	if (StorageType == EStorageType::BackPackSlot)
	{
		APSE_LYFE_Inventory2_Storage* BackPackPtr = Cast<APSE_LYFE_Inventory2_Storage>(this);
		FItemStruct TempItemStruct = StoredItem;
		BackPackPtr->DeleteItems(StoredLoc);
		StoredItem = CursorItem;
		if (CursorItem.ItemProperties.IsValidIndex(0))
		{
			BackPackPtr->ItemAdded(CursorItem.ItemClass, CursorItem.ItemProperties[0]);
		}
		else
		{
			BackPackPtr->ItemAdded(CursorItem.ItemClass);
		}
		CursorItem = TempItemStruct;
	}
	else if (StorageType == EStorageType::StorageSlot)
	{
		APSE_LYFE_Inventory5_ExterStorage* StoragePtr = Cast<APSE_LYFE_Inventory5_ExterStorage>(LastStoragePtr);
		FItemStruct TempItemStruct = StoredItem;
		StoragePtr->DeleteItemsStorage(StoredLoc);
		StoredItem = CursorItem;
		CursorItem = TempItemStruct;
	}
}

void APSE_LYFE_Inventory1_Cursor::ItemPicked(FStorageLoc StoredLoc, EStorageType StorageType)
{
	LastStoragePtr = this;
	LastStoredStorageLoc = StoredLoc;
	LastStoragePtrType = StorageType;
}

void APSE_LYFE_Inventory1_Cursor::ItemPicked(uint8 EquipmentSlotLoc)
{
	LastStoragePtr = this;
	LastStoredEquipmentLoc = EquipmentSlotLoc;
	LastStoragePtrType = EStorageType::EquipmentSlot;
}

void APSE_LYFE_Inventory1_Cursor::ResetItemLastLocation()
{
	if (CursorItem.ItemClass != nullptr)
	{
		Server_ResetItemLastLocation();
	}
}

bool APSE_LYFE_Inventory1_Cursor::Server_ResetItemLastLocation_Validate()
{
	return true;
}

void APSE_LYFE_Inventory1_Cursor::Server_ResetItemLastLocation_Implementation()
{
	if (LastStoragePtr && CursorItem.ItemClass != nullptr)
	{
		if (LastStoragePtrType == EStorageType::BackPackSlot)
		{
			APSE_LYFE_Inventory2_Storage* TempBackPackPtr = Cast<APSE_LYFE_Inventory2_Storage>(LastStoragePtr);
			TempBackPackPtr->AddItem(CursorItem, LastStoredStorageLoc);
		}
		else if (LastStoragePtrType == EStorageType::EquipmentSlot)
		{
			APSE_LYFE_Inventory3_Equipment* TempStoragePtr = Cast<APSE_LYFE_Inventory3_Equipment>(LastStoragePtr);
			TempStoragePtr->AddInventoryEquipment(CursorItem, LastStoredEquipmentLoc);
		}
		else if (LastStoragePtrType == EStorageType::StorageSlot)
		{
			APSE_LYFE_Inventory5_ExterStorage* TempStoragePtr = Cast<APSE_LYFE_Inventory5_ExterStorage>(LastStoragePtr);
			TempStoragePtr->AddItemStorage(CursorItem, LastStoredStorageLoc);
		}
	}
}


void APSE_LYFE_Inventory1_Cursor::ThrowAllCursorItems()
{
	if (CursorItem.ItemClass != nullptr)
	{
		FTimerHandle ThrowItemsHandle;
		GetWorldTimerManager().SetTimer(ThrowItemsHandle, this, &APSE_LYFE_Inventory1_Cursor::ThrowAllCursorItems, 0.15, false);
		ThrowCursorItem();
	}
	else
	{
		bIsCursorFunctional = true;
	}
}

void APSE_LYFE_Inventory1_Cursor::ThrowCursorItem()
{
	if (CursorItem.ItemClass == nullptr)
	{
		return;
	}
	else
	{
		Server_ThrowCursorItem();
	}
}

bool APSE_LYFE_Inventory1_Cursor::Server_ThrowCursorItem_Validate()
{
	return true;
}

void APSE_LYFE_Inventory1_Cursor::Server_ThrowCursorItem_Implementation()
{
	if (CursorItem.ItemClass == nullptr)
	{
		return;
	}
	const APSE_LYFE_BaseInventoryItem* ThrownItem = CursorItem.GetDefaultItem();
	if (ThrownItem->GetItemType() == EItemType::StackableItem || ThrownItem->GetItemType() == EItemType::StackableUsableItem)
	{
		int32 &CurrentStacks = CursorItem.ItemProperties[0];
		if (CurrentStacks > 0)
		{
			const FVector GetItemThrowLocation = OwningPawn->GetCharacterThrowLocation();
			CursorItem.CreateItem(GetWorld(), GetItemThrowLocation);
			if (CurrentStacks == 1)
			{
				CursorItem.EmptyItem();
			}
			else
			{
				CurrentStacks -= 1;
			}
		}
	}
	else
	{
		const FVector GetItemThrowLocation = OwningPawn->GetCharacterThrowLocation();
		CursorItem.CreateItem(GetWorld(), GetItemThrowLocation);
		CursorItem.EmptyItem();
	}
}

void APSE_LYFE_Inventory1_Cursor::AltPressed()
{
	if (bIsAltPressed == false)
	{
		bIsAltPressed = true;
		Server_AltPressed();
	}
}

bool APSE_LYFE_Inventory1_Cursor::Server_AltPressed_Validate()
{
	return true;
}

void APSE_LYFE_Inventory1_Cursor::Server_AltPressed_Implementation()
{
	bIsAltPressed = true;
}

void APSE_LYFE_Inventory1_Cursor::AltReleased()
{
	if (bIsAltPressed == true)
	{
		bIsAltPressed = false;
		Server_AltReleased();
	}
}

bool APSE_LYFE_Inventory1_Cursor::Server_AltReleased_Validate()
{
	return true;
}

void APSE_LYFE_Inventory1_Cursor::Server_AltReleased_Implementation()
{
	bIsAltPressed = false;
}

void APSE_LYFE_Inventory1_Cursor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Inventory1_Cursor, CursorItem, COND_OwnerOnly);
}