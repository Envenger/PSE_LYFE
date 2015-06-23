// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "UnrealNetwork.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "PSE_LYFE_Inventory5_ExterStorage.h"

APSE_LYFE_Inventory5_ExterStorage::APSE_LYFE_Inventory5_ExterStorage()
{
	OpenedStorage = nullptr;
}

void APSE_LYFE_Inventory5_ExterStorage::OnRep_StoragePointerChange()
{
	if (OpenedStorage != nullptr)
	{
		if (InventoryState == EInventoryState::InventoryOpen)
		{
			CharacterHUD->CloseInventoryUI();
		}
		if (CharacterHUD->CreateStorageUI())
		{
			InventoryState = EInventoryState::StorageOpen;
		}
	}
	else
	{
		InventoryState = EInventoryState::Close;
	}
}

void APSE_LYFE_Inventory5_ExterStorage::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APSE_LYFE_Inventory5_ExterStorage::CloseStorage()
{
	CharacterHUD->CloseStorageUI();
	Server_CloseStorage();
}

bool APSE_LYFE_Inventory5_ExterStorage::Server_CloseStorage_Validate()
{
	return true;
}
void APSE_LYFE_Inventory5_ExterStorage::Server_CloseStorage_Implementation()
{
	OpenedStorage = nullptr;
}

void APSE_LYFE_Inventory5_ExterStorage::StorageSlotLeftClick(const FStorageLoc ItemLoc)
{
	if (bIsCursorFunctional && OpenedStorage && OpenedStorage->Storage.HasValidItem(ItemLoc))
	{
		Server_StorageSlotLeftClick(ItemLoc);
	}
}

bool APSE_LYFE_Inventory5_ExterStorage::Server_StorageSlotLeftClick_Validate(const FStorageLoc ItemLoc)
{
	return true;
}
void APSE_LYFE_Inventory5_ExterStorage::Server_StorageSlotLeftClick_Implementation(const FStorageLoc ItemLoc)
{
	if (bIsCursorFunctional && OpenedStorage && OpenedStorage->Storage.HasValidItem(ItemLoc))
	{
		if (bIsAltPressed == false)
		{
			if (CursorItem.ItemClass == nullptr)
			{
				if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass != nullptr)
				{
					AddItemToCursor(OpenedStorage->Storage.GetItem(ItemLoc), ItemLoc, EStorageType::StorageSlot);
				}
			}
			else
			{
				if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass == nullptr)
				{
					AddItemStorage(CursorItem, ItemLoc);
				}
				else if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass == CursorItem.ItemClass)
				{
					const APSE_LYFE_BaseInventoryItem* BaseItem = CursorItem.GetDefaultItem();
					if (BaseItem->GetItemType() == EItemType::StackableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
					{
						AddItemStorage(CursorItem, ItemLoc);
					}
					else if (OpenedStorage->Storage.GetItem(ItemLoc).ItemProperties != CursorItem.ItemProperties)
					{
						CursorSwap(OpenedStorage->Storage.GetItem(ItemLoc), ItemLoc, EStorageType::StorageSlot);
					}
				}
				else
				{
					CursorSwap(OpenedStorage->Storage.GetItem(ItemLoc), ItemLoc, EStorageType::StorageSlot);
				}
			}
		}
		else
		{
			if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass != nullptr)
			{
				AddItemToCursorAlt(OpenedStorage->Storage.GetItem(ItemLoc), ItemLoc, EStorageType::StorageSlot);
			}
		}
	}
}

void APSE_LYFE_Inventory5_ExterStorage::StorageSlotRightClick(const FStorageLoc ItemLoc)
{
	if (bIsCursorFunctional && OpenedStorage && OpenedStorage->Storage.HasValidItem(ItemLoc))
	{
		Server_StorageSlotRightClick(ItemLoc);
	}
}

bool APSE_LYFE_Inventory5_ExterStorage::Server_StorageSlotRightClick_Validate(const FStorageLoc ItemLoc)
{
	return true;
}
void APSE_LYFE_Inventory5_ExterStorage::Server_StorageSlotRightClick_Implementation(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !OpenedStorage->Storage.HasValidItem(ItemLoc))
	{
		return;
	}
	if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass == nullptr)
	{
		ResetItemLastLocation();
		return;
	}
	AddItem(OpenedStorage->Storage.GetItem(ItemLoc));
}

const FStorageLoc APSE_LYFE_Inventory5_ExterStorage::FindFirstEmptySlotStorage()
{
	for (uint16 i = 0; i < OpenedStorage->StorageBase.Num(); i++)
	{
		if (OpenedStorage->StorageBase[i].ItemClass == nullptr)
		{
			return OpenedStorage->Storage.GetStorageLocFromIndex(i);
		}
	}
	return FStorageLoc(-1, -1);
}

const FStorageLoc APSE_LYFE_Inventory5_ExterStorage::FindFirstFreeSlotStorage(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass)
{
	const APSE_LYFE_BaseInventoryItem* BaseItem = ItemClass.GetDefaultObject();
	if (BaseItem->GetItemType() != EItemType::StackableItem && BaseItem->GetItemType() != EItemType::StackableUsableItem)
	{
		return FindFirstEmptySlotStorage();
	}
	for (uint16 i = 0; i < OpenedStorage->StorageBase.Num(); i++)
	{
		if (ItemClass == OpenedStorage->StorageBase[i].ItemClass)
		{
			const int32 MaxStacks = BaseItem->GetMaxStacks();
			int32 CurrentStacks = OpenedStorage->StorageBase[i].ItemProperties[0];
			if (CurrentStacks < MaxStacks)
			{
				return OpenedStorage->Storage.GetStorageLocFromIndex(i);
			}
		}
	}
	return FStorageLoc(-1, -1);
}

void APSE_LYFE_Inventory5_ExterStorage::DeleteItemsStorage(const FStorageLoc ItemLoc, const int32 Stacks)
{
	const APSE_LYFE_BaseInventoryItem* DeletingItemType = OpenedStorage->Storage.GetItem(ItemLoc).GetDefaultItem();
	if (DeletingItemType->GetItemType() == EItemType::StackableItem || DeletingItemType->GetItemType() == EItemType::StackableUsableItem)
	{
		int32 &CurrentStacks = OpenedStorage->Storage.GetItem(ItemLoc).ItemProperties[0];
		if (CurrentStacks - Stacks > 0 && Stacks > -1)// Items will remain even after deletions
		{
			CurrentStacks -= Stacks;
		}
		else // Item needs to be completely removed
		{
			OpenedStorage->Storage.GetItem(ItemLoc).EmptyItem();
		}
	}
	else if (Stacks != 0)
	{
		OpenedStorage->Storage.GetItem(ItemLoc).EmptyItem();
	}
}

const bool APSE_LYFE_Inventory5_ExterStorage::AddItemStorage(FItemStruct &ItemStruct, const FStorageLoc ItemLoc)
{
	if (ItemStruct.ItemClass == nullptr)
	{
		return false;
	}
	const APSE_LYFE_BaseInventoryItem* AddedItem = ItemStruct.GetDefaultItem();
	if (AddedItem->GetItemType() == EItemType::StackableItem || AddedItem->GetItemType() == EItemType::StackableUsableItem)
	{
		if (ItemStruct.ItemProperties[0] <= 0)
		{
			return false;
		}
		if (ItemLoc.RowNum <= -1 || ItemLoc.ColNum <= -1) // No particular slots are allocated for the item OpenedStorage->Storage
		{
			const FStorageLoc FirstFreeItemLoc = FindFirstFreeSlotStorage(ItemStruct.ItemClass);
			if (FirstFreeItemLoc.RowNum > -1 && FirstFreeItemLoc.ColNum > -1) //Empty or existing items present
			{
				const int32 MaxStacks = AddedItem->GetMaxStacks();
				int32 &StoredCurrentStacks = OpenedStorage->Storage.GetItem(FirstFreeItemLoc).ItemProperties[0];
				int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
				const int32 ItemsToAdd = FMath::Min((MaxStacks - StoredCurrentStacks), AddedCurrentStacks);
				StoredCurrentStacks += ItemsToAdd;

				AddedCurrentStacks -= ItemsToAdd;
				if (AddedCurrentStacks > 0) // Does stacks remain after adding
				{
					AddItemStorage(ItemStruct);
				}
				else
				{
					ItemStruct.EmptyItem();
				}
			}
			else // No empty or existing items present
			{
				const FStorageLoc FirstEmptyItemLoc = FindFirstEmptySlotStorage();
				if (FirstEmptyItemLoc.RowNum <= -1 || FirstEmptyItemLoc.ColNum <= -1) // Inventory full
				{
					GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Inventory full");
					return false;
				}
				else // Empty location found
				{
					const int32 MaxStacks = AddedItem->GetMaxStacks();
					int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
					const int32 ItemsToAdd = FMath::Min(MaxStacks, AddedCurrentStacks);

					OpenedStorage->Storage.GetItem(FirstEmptyItemLoc) = ItemStruct;
					OpenedStorage->Storage.GetItem(FirstEmptyItemLoc).ItemProperties[0] = ItemsToAdd;

					AddedCurrentStacks -= ItemsToAdd;
					if (AddedCurrentStacks > 0)// Does stacks remain after adding
					{
						AddItemStorage(ItemStruct);
					}
					else
					{
						ItemStruct.EmptyItem();
					}
				}
			}
		}
		else // Location already given
		{
			if (ItemLoc.RowNum > (OpenedStorage->StorageDisplaySize.RowNum - 1) && ItemLoc.ColNum > (OpenedStorage->StorageDisplaySize.ColNum - 1)) //Is Location within bounds
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OutOfBounds-- " + FString::FromInt(ItemLoc.RowNum) + " "
					+ FString::FromInt(ItemLoc.ColNum));
				return false;
			}
			else
			{
				if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass != nullptr) // Is Location empty (triggered when adding item by mouse)
				{
					if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass == ItemStruct.ItemClass)
					{
						const int32 MaxStacks = AddedItem->GetMaxStacks();
						int32 &StoredCurrentStacks = OpenedStorage->Storage.GetItem(ItemLoc).ItemProperties[0];
						int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
						const int32 ItemsToAdd = FMath::Min((MaxStacks - StoredCurrentStacks), AddedCurrentStacks);

						StoredCurrentStacks += ItemsToAdd;

						AddedCurrentStacks -= ItemsToAdd;
						if (AddedCurrentStacks <= 0)
						{
							ItemStruct.EmptyItem();
						}
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Red, "Class No compatible");
					}
				}
				else // Location empty
				{
					const int32 MaxStacks = AddedItem->GetMaxStacks();
					int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
					const int32 ItemsToAdd = FMath::Min(MaxStacks, AddedCurrentStacks);

					OpenedStorage->Storage.GetItem(ItemLoc) = ItemStruct;
					OpenedStorage->Storage.GetItem(ItemLoc).ItemProperties[0] = ItemsToAdd;

					AddedCurrentStacks -= ItemsToAdd;
					if (AddedCurrentStacks <= 0)// Does stacks remain after adding
					{
						ItemStruct.EmptyItem();
					}
				}
			}
		}
	}
	else
	{
		if (ItemLoc.RowNum <= -1 || ItemLoc.ColNum <= -1)// No particular slots are allocated for the item OpenedStorage->Storage
		{
			const FStorageLoc FirstEmptyItemLoc = FindFirstEmptySlotStorage();
			if (FirstEmptyItemLoc.RowNum <= -1 || FirstEmptyItemLoc.ColNum <= -1) // Inventory full
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Inventory full");
				return false;
			}
			else // Empty location found
			{
				OpenedStorage->Storage.GetItem(FirstEmptyItemLoc) = ItemStruct;

				ItemStruct.EmptyItem();
			}
		}
		else
		{
			if (ItemLoc.RowNum > (OpenedStorage->StorageDisplaySize.RowNum - 1) && ItemLoc.ColNum > (OpenedStorage->StorageDisplaySize.ColNum - 1)) //Is Location within bounds
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OutOfBounds-- " + FString::FromInt(ItemLoc.RowNum) + " "
					+ FString::FromInt(ItemLoc.ColNum));
				return false;
			}
			else
			{
				if (OpenedStorage->Storage.GetItem(ItemLoc).ItemClass == nullptr) // Is Location empty (triggered when adding item by mouse)
				{
					OpenedStorage->Storage.GetItem(ItemLoc) = ItemStruct;

					ItemStruct.EmptyItem();
				}
			}
		}
	}
	return true;
}

void APSE_LYFE_Inventory5_ExterStorage::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Inventory5_ExterStorage, OpenedStorage, COND_OwnerOnly);
}