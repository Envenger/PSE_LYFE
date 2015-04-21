// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_InventoryStructures.h"
#include "PSE_LYFE_Inventory2_Storage.h"
#include "PSE_LYFE_Inventory3_Equipment.h"
#include "Player/PSE_LYFE_Character.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Inventory1_Cursor.h"


// Sets default values
APSE_LYFE_Inventory1_Cursor::APSE_LYFE_Inventory1_Cursor()
{
	CursorItem.ItemClass = nullptr;

	bIsCursorFunctional = true;
	bIsAltPressed = false;
	LastStoragePtrType = ELastClickedStorageType::Nothing;
}


void APSE_LYFE_Inventory1_Cursor::AddItemToCursor(FItemStruct& StoredItem, FStorageLoc StoredLoc)
{
	if (this->IsA(APSE_LYFE_Inventory2_Storage::StaticClass()))
	{
		APSE_LYFE_Inventory2_Storage* StoragePtr = Cast<APSE_LYFE_Inventory2_Storage>(this);
		CursorItem = StoredItem;
		StoragePtr->DeleteItems(StoredLoc, -1); // All items deleted                                                                                                  
		ItemPicked(StoragePtr, StoredLoc);
	}
}

void APSE_LYFE_Inventory1_Cursor::AddItemToCursor(FItemStruct& StoredItem, uint8 EquipmentSlotLoc)
{
	if (this->IsA(APSE_LYFE_Inventory3_Equipment::StaticClass()))
	{
		APSE_LYFE_Inventory3_Equipment* EquipmentPtr = Cast<APSE_LYFE_Inventory3_Equipment>(this);
		CursorItem = StoredItem;
		EquipmentPtr->RemoveInventoryEquipment(EquipmentSlotLoc);
		ItemPicked(EquipmentPtr, EquipmentSlotLoc);
	}
}

void APSE_LYFE_Inventory1_Cursor::AddItemToCursorAlt(FItemStruct& StoredItem, FStorageLoc StoredLoc)
{
	if (this->IsA(APSE_LYFE_Inventory2_Storage::StaticClass()))
	{
		APSE_LYFE_Inventory2_Storage* StoragePtr = Cast<APSE_LYFE_Inventory2_Storage>(this);
		if (CursorItem.ItemClass == nullptr)
		{
			if (StoredItem.ItemClass != nullptr)
			{
				const APSE_LYFE_BaseInventoryItem* BaseItem = StoragePtr->Storage.GetItem(StoredLoc).GetDefaultItem();
				if (BaseItem->ItemType == EItemType::StackableItem || BaseItem->ItemType == EItemType::StackableUsableItem)
				{
					int32 &CurrentStacks = StoredItem.ItemProperties[0];
					CursorItem = StoredItem;
					CursorItem.ItemProperties[0] = 1;
					StoragePtr->DeleteItems(StoredLoc, 1); // Items deleted by 1 stack
					ItemPicked(StoragePtr, StoredLoc);
				}
				else
				{
					AddItemToCursor(StoredItem, StoredLoc);
				}
			}
		}
		else if (StoredItem.ItemClass == CursorItem.ItemClass)
		{
			const APSE_LYFE_BaseInventoryItem* BaseItem = CursorItem.GetDefaultItem();
			const int32 MaxStacks = BaseItem->MaxStacks;
			int32 &CurrentStacks = CursorItem.ItemProperties[0];
			if (CurrentStacks < MaxStacks)
			{
				CurrentStacks += 1;
				StoragePtr->DeleteItems(StoredLoc, 1); // Item delete by 1 stack
				ItemPicked(StoragePtr, StoredLoc);
			}
		}
	}
}

void APSE_LYFE_Inventory1_Cursor::CursorSwap(FItemStruct& StoredItem, FStorageLoc StoredLoc)
{
	if (this->IsA(APSE_LYFE_Inventory2_Storage::StaticClass()))
	{
		APSE_LYFE_Inventory2_Storage* StoragePtr = Cast<APSE_LYFE_Inventory2_Storage>(this);
		FItemStruct TempItemStruct = StoredItem;
		StoragePtr->DeleteItems(StoredLoc);
		StoredItem = CursorItem;
		if (CursorItem.ItemProperties.IsValidIndex(0))
		{
			StoragePtr->ItemAdded(CursorItem.ItemClass, CursorItem.ItemProperties[0]);
		}
		else
		{
			StoragePtr->ItemAdded(CursorItem.ItemClass);
		}
		CursorItem = TempItemStruct;
	}
}

void APSE_LYFE_Inventory1_Cursor::ItemPicked(AActor* StoragePtr, FStorageLoc StoredLoc)
{
	LastStoragePtr = StoragePtr;
	LastStoredStorageLoc = StoredLoc;
	LastStoragePtrType = ELastClickedStorageType::StorageSlot;
}

void APSE_LYFE_Inventory1_Cursor::ItemPicked(AActor* StoragePtr, uint8 EquipmentSlotLoc)
{
	LastStoragePtr = StoragePtr;
	LastStoredEquipmentLoc = EquipmentSlotLoc;
	LastStoragePtrType = ELastClickedStorageType::EquipmentSlot;
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
		if (LastStoragePtrType == ELastClickedStorageType::StorageSlot)
		{
			APSE_LYFE_Inventory2_Storage* TempStoragePtr = Cast<APSE_LYFE_Inventory2_Storage>(LastStoragePtr);
			TempStoragePtr->AddItem(CursorItem, LastStoredStorageLoc);
		}
		else if (LastStoragePtrType == ELastClickedStorageType::EquipmentSlot)
		{
			APSE_LYFE_Inventory3_Equipment* TempStoragePtr = Cast<APSE_LYFE_Inventory3_Equipment>(LastStoragePtr);
			TempStoragePtr->AddInventoryEquipment(CursorItem, LastStoredEquipmentLoc);
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
	if (ThrownItem->ItemType == EItemType::StackableItem || ThrownItem->ItemType == EItemType::StackableUsableItem)
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