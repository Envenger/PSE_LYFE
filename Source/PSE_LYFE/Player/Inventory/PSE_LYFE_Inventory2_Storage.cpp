// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Items/Ammo/PSE_LYFE_BaseWeaponMagazine.h"
#include "PSE_LYFE_Inventory5_ExterStorage.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Inventory2_Storage.h"
//#include "Weapons/BaseFiles/PSE_LYFE_ReloadableWeapon.h"
APSE_LYFE_Inventory2_Storage::APSE_LYFE_Inventory2_Storage()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultBagPackSize = 5;
	BackPackSize = 0;//Initializing default bag size

	InventoryWidth = 500;
	InventoryIconSize = 65;

	TotalItemsWeight = 0;
}




void APSE_LYFE_Inventory2_Storage::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetBackPackSize();
}

const uint16 APSE_LYFE_Inventory2_Storage::GetTotalBackPackSize() const
{
	return DefaultBagPackSize + BackPackSize;
}

void APSE_LYFE_Inventory2_Storage::SetBackPackSize()
{
	uint16 TotalStorageSize = GetTotalBackPackSize();
	int32 NoOfRows = FMath::CeilToInt((float)TotalStorageSize / 5);
	if (NoOfRows < 5)
	{
		NoOfRows = 5;
	}
	BackPackDisplaySize = FStorageLoc(NoOfRows, 5);
	BackPack.AddUninitialized(NoOfRows, 5);
	BackPack.InitializeArray(BackPackBase, TotalStorageSize);
}

void APSE_LYFE_Inventory2_Storage::ResetBackPackSize(const int16 NewBagSize)
{
	uint16 NewTotalStorageSize = NewBagSize + DefaultBagPackSize;
	uint16 OldTotalStorageSize = GetTotalBackPackSize();
	int16 NoOfRows = FMath::CeilToInt((float)NewTotalStorageSize / 5);
	if (NoOfRows < 5)
	{
		NoOfRows = 5;
	}
	FStorageLoc NewStorageDisplaySize = FStorageLoc(NoOfRows, 5);
		if (NewTotalStorageSize < OldTotalStorageSize)
	{
		int16 ElementsToRemove = OldTotalStorageSize - NewTotalStorageSize;
		BackPackBase.RemoveAt(NewTotalStorageSize, ElementsToRemove);
	}
	else
	{
		int16 ElementsToAdd = NewTotalStorageSize - OldTotalStorageSize;
		FItemStruct TempStruct;
		TempStruct.ItemClass = nullptr;
		for (int16 i = ElementsToAdd; i > 0; i--)
		{
			BackPackBase.Add(TempStruct);
		}
	}
	FStorageArray TempStorageArray;
	TempStorageArray.StorageBasePtr = &BackPackBase;
	TempStorageArray.AddUninitialized(NewStorageDisplaySize.RowNum, 5);
	uint16 SlotsAssigned = 0;
	for (int8 i = 0; i < TempStorageArray.Rows.Num(); i++)
	{
		for (int8 j = 0; j < 5; j++)
		{
			if (SlotsAssigned < NewTotalStorageSize)
			{
				TempStorageArray.Rows[i].Columns[j].Index = SlotsAssigned;
				SlotsAssigned++;
			}
			else
			{
				TempStorageArray.Rows[i].Columns[j].Index = -1;
			}
		}
	}
	BackPack = TempStorageArray;
	BackPackSize = NewBagSize;
	BackPackDisplaySize = NewStorageDisplaySize;
}

const int16 APSE_LYFE_Inventory2_Storage::GetLowestItemIndex() const
{
	for (int16 i = (BackPackBase.Num() - 1); i >= 0; i--)
	{
		if (BackPackBase[i].ItemClass != nullptr)
		{
			return i;
		}
	}
	return -1;
}

void APSE_LYFE_Inventory2_Storage::OnRep_ClientResetBagSize()
{
	NoOfSlotsUsed = GetNoOfUsedSlots();
	if (BackPackBase.Num() != GetTotalBackPackSize())
	{
		if (InventoryState == EInventoryState::InventoryOpen)
		{
			CharacterHUD->CloseTempBackPackSlot();
		}
		int16 NewStorageSize = BackPackBase.Num();

		int16 NoOfRows = FMath::CeilToInt((float)NewStorageSize / 5);
		if (NoOfRows < 5)
		{
			NoOfRows = 5;
		}
		FStorageLoc NewStorageDisplaySize = FStorageLoc(NoOfRows, 5);

		FStorageArray TempStorageArray;
		TempStorageArray.StorageBasePtr = &BackPackBase;
		TempStorageArray.AddUninitialized(NewStorageDisplaySize.RowNum, 5);
		uint16 SlotsAssigned = 0;
		for (int8 i = 0; i < TempStorageArray.Rows.Num(); i++)
		{
			for (int8 j = 0; j < 5; j++)
			{
				if (SlotsAssigned < NewStorageSize)
				{
					TempStorageArray.Rows[i].Columns[j].Index = SlotsAssigned;
					SlotsAssigned++;
				}
				else
				{
					TempStorageArray.Rows[i].Columns[j].Index = -1;
				}
			}
		}
		BackPack = TempStorageArray;
		BackPackSize = NewStorageSize - DefaultBagPackSize;
		BackPackDisplaySize = NewStorageDisplaySize;
		if (InventoryState == EInventoryState::InventoryOpen)
		{
			CharacterHUD->CreateTempBackPackSlot();
		}
	}
}

void APSE_LYFE_Inventory2_Storage::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle TestHandle;
	//GetWorldTimerManager().SetTimer(TestHandle, this, &APSE_LYFE_Inventory2_Storage::TestFunction, 5, true);
}

void APSE_LYFE_Inventory2_Storage::TestFunction()
{
	if (Role == ROLE_Authority)
	{
		uint8 NewBagSize = FMath::RandRange(0, 20);
		if (NewBagSize != BackPackSize)
		{
			ResetBackPackSize(NewBagSize);
		}
	}
}

void APSE_LYFE_Inventory2_Storage::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void APSE_LYFE_Inventory2_Storage::BackPackSlotLeftClick(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !BackPack.HasValidItem(ItemLoc))
	{
		return;
	}
	Server_BackPackSlotLeftClick(ItemLoc);
}

bool APSE_LYFE_Inventory2_Storage::Server_BackPackSlotLeftClick_Validate(const FStorageLoc ItemLoc)
{
	return true;
}

void APSE_LYFE_Inventory2_Storage::Server_BackPackSlotLeftClick_Implementation(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !BackPack.HasValidItem(ItemLoc))
	{
		return;
	}
	if (bIsAltPressed == false)
	{
		if (CursorItem.ItemClass == nullptr)
		{
			if (BackPack.GetItem(ItemLoc).ItemClass != nullptr)
			{
				AddItemToCursor(BackPack.GetItem(ItemLoc), ItemLoc, EStorageType::BackPackSlot);
			}
		}
		else
		{
			if (BackPack.GetItem(ItemLoc).ItemClass == nullptr)
			{
				AddItem(CursorItem, ItemLoc);
			}
			else if (BackPack.GetItem(ItemLoc).ItemClass == CursorItem.ItemClass)
			{
				const APSE_LYFE_BaseInventoryItem* BaseItem = CursorItem.GetDefaultItem();
				if (BaseItem->GetItemType() == EItemType::StackableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
				{
					AddItem(CursorItem, ItemLoc);
				}
				else if (BackPack.GetItem(ItemLoc).ItemProperties != CursorItem.ItemProperties)
				{
					CursorSwap(BackPack.GetItem(ItemLoc), ItemLoc, EStorageType::BackPackSlot);
				}
			}
			else
			{
				CursorSwap(BackPack.GetItem(ItemLoc), ItemLoc, EStorageType::BackPackSlot);
			}
		}
	}
	else
	{
		if (BackPack.GetItem(ItemLoc).ItemClass != nullptr)
		{
			AddItemToCursorAlt(BackPack.GetItem(ItemLoc), ItemLoc, EStorageType::BackPackSlot);
		}
	}
	
}

void APSE_LYFE_Inventory2_Storage::BackPackSlotRightClick(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !BackPack.HasValidItem(ItemLoc))
	{
		return;
	}
	Server_BackPackSlotRightClick(ItemLoc);
}

bool APSE_LYFE_Inventory2_Storage::Server_BackPackSlotRightClick_Validate(const FStorageLoc ItemLoc)
{
	return true;
}

void APSE_LYFE_Inventory2_Storage::Server_BackPackSlotRightClick_Implementation(const FStorageLoc ItemLoc)
{
	if (bIsCursorFunctional && BackPack.HasValidItem(ItemLoc))
	{
		APSE_LYFE_Inventory5_ExterStorage* ExternalStorage = Cast<APSE_LYFE_Inventory5_ExterStorage>(this);
		if (ExternalStorage->OpenedStorage)// If Storage is open
		{
			ExternalStorage->AddItemStorage(BackPack.GetItem(ItemLoc));
		}
		else
		{
			if (BackPack.GetItem(ItemLoc).ItemClass == nullptr)
			{
				ResetItemLastLocation();
				return;
			}
			const APSE_LYFE_BaseInventoryItem* BaseItem = BackPack.GetItem(ItemLoc).GetDefaultItem();
			if (BaseItem->GetItemType() == EItemType::UsableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
			{
				BaseItem->UseItem(OwningPawn);
				DeleteItems(ItemLoc, 1); // Use Item
			}
			else if (BaseItem->GetItemType() == EItemType::EquipableItem)
			{
				EquipItem(ItemLoc);
			}
		}
	}
}

const FStorageLoc APSE_LYFE_Inventory2_Storage::FindFirstEmptySlot()
{
	for (uint16 i = 0; i < BackPackBase.Num(); i++)
	{
		if (BackPackBase[i].ItemClass == nullptr)
		{
			return BackPack.GetStorageLocFromIndex(i);
		}
	}
	return FStorageLoc(-1, -1);
}

const FStorageLoc APSE_LYFE_Inventory2_Storage::FindFirstFreeSlot(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass)
{
	const APSE_LYFE_BaseInventoryItem* BaseItem = ItemClass.GetDefaultObject();
	if (BaseItem->GetItemType() != EItemType::StackableItem && BaseItem->GetItemType() != EItemType::StackableUsableItem)
	{
		return FindFirstEmptySlot();
	}
	for (uint16 i = 0; i < BackPackBase.Num(); i++)
	{
		if (ItemClass == BackPackBase[i].ItemClass)
		{
			const int32 MaxStacks = BaseItem->GetMaxStacks();
			int32 CurrentStacks = BackPackBase[i].ItemProperties[0];
			if (CurrentStacks < MaxStacks)
			{
				return BackPack.GetStorageLocFromIndex(i);
			}
		}
	}
	return FStorageLoc(-1, -1);
}


void APSE_LYFE_Inventory2_Storage::DeleteItems(const FStorageLoc ItemLoc, const int32 Stacks)
{
	const APSE_LYFE_BaseInventoryItem* DeletingItemType = BackPack.GetItem(ItemLoc).GetDefaultItem();
	if (DeletingItemType->GetItemType() == EItemType::StackableItem || DeletingItemType->GetItemType() == EItemType::StackableUsableItem)
	{
		int32 &CurrentStacks = BackPack.GetItem(ItemLoc).ItemProperties[0];
		if (CurrentStacks - Stacks > 0 && Stacks > -1)// Items will remain even after deletions
		{
			ItemRemoved(BackPack.GetItem(ItemLoc).ItemClass, Stacks);
			CurrentStacks -= Stacks;
		}
		else // Item needs to be completely removed
		{
			ItemRemoved(BackPack.GetItem(ItemLoc).ItemClass, BackPack.GetItem(ItemLoc).ItemProperties[0]);
			BackPack.GetItem(ItemLoc).EmptyItem();
		}
	}
	else if (Stacks != 0)
	{
		ItemRemoved(BackPack.GetItem(ItemLoc).ItemClass);
		BackPack.GetItem(ItemLoc).EmptyItem();
	}
}


const bool APSE_LYFE_Inventory2_Storage::AddItem(FItemStruct &ItemStruct, const FStorageLoc ItemLoc)
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
		if (ItemLoc.RowNum <= -1 || ItemLoc.ColNum <= -1) // No particular slots are allocated for the item storage
		{
			const FStorageLoc FirstFreeItemLoc = FindFirstFreeSlot(ItemStruct.ItemClass);
			if (FirstFreeItemLoc.RowNum > -1 && FirstFreeItemLoc.ColNum > -1) //Empty or existing items present
			{
				const int32 MaxStacks = AddedItem->GetMaxStacks();
				int32 &StoredCurrentStacks = BackPack.GetItem(FirstFreeItemLoc).ItemProperties[0];
				int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
				const int32 ItemsToAdd = FMath::Min((MaxStacks - StoredCurrentStacks), AddedCurrentStacks);

				StoredCurrentStacks += ItemsToAdd;
				ItemAdded(ItemStruct.ItemClass, ItemsToAdd); // Items added

				AddedCurrentStacks -= ItemsToAdd;
				if (AddedCurrentStacks > 0) // Does stacks remain after adding
				{
					AddItem(ItemStruct);
				}
				else
				{
					ItemStruct.EmptyItem();
				}
			}
			else // No empty or existing items present
			{
				const FStorageLoc FirstEmptyItemLoc = FindFirstEmptySlot();
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

					BackPack.GetItem(FirstEmptyItemLoc) = ItemStruct;
					BackPack.GetItem(FirstEmptyItemLoc).ItemProperties[0] = ItemsToAdd;
					ItemAdded(ItemStruct.ItemClass, ItemsToAdd); // Items added

					AddedCurrentStacks -= ItemsToAdd;
					if (AddedCurrentStacks > 0)// Does stacks remain after adding
					{
						AddItem(ItemStruct);
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
			if (ItemLoc.RowNum > (BackPackDisplaySize.RowNum - 1) && ItemLoc.ColNum > (BackPackDisplaySize.ColNum - 1)) //Is Location within bounds
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OutOfBounds-- " + FString::FromInt(ItemLoc.RowNum) + " "
					+ FString::FromInt(ItemLoc.ColNum));
				return false;
			}
			else
			{
				if (BackPack.GetItem(ItemLoc).ItemClass != nullptr) // Is Location empty (triggered when adding item by mouse)
				{
					if (BackPack.GetItem(ItemLoc).ItemClass == ItemStruct.ItemClass)
					{
						const int32 MaxStacks = AddedItem->GetMaxStacks();
						int32 &StoredCurrentStacks = BackPack.GetItem(ItemLoc).ItemProperties[0];
						int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
						const int32 ItemsToAdd = FMath::Min((MaxStacks - StoredCurrentStacks), AddedCurrentStacks);

						StoredCurrentStacks += ItemsToAdd;
						ItemAdded(ItemStruct.ItemClass, ItemsToAdd); // Items added

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

					BackPack.GetItem(ItemLoc) = ItemStruct;
					BackPack.GetItem(ItemLoc).ItemProperties[0] = ItemsToAdd;
					ItemAdded(ItemStruct.ItemClass, ItemsToAdd); // Items added

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
		if (ItemLoc.RowNum <= -1 || ItemLoc.ColNum <= -1)// No particular slots are allocated for the item storage
		{
			const FStorageLoc FirstEmptyItemLoc = FindFirstEmptySlot();
			if (FirstEmptyItemLoc.RowNum <= -1 || FirstEmptyItemLoc.ColNum <= -1) // Inventory full
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Inventory full");
				return false;
			}
			else // Empty location found
			{
				BackPack.GetItem(FirstEmptyItemLoc) = ItemStruct;
				ItemAdded(ItemStruct.ItemClass); // Items Added

				ItemStruct.EmptyItem();
			}
		}
		else
		{
			if (ItemLoc.RowNum > (BackPackDisplaySize.RowNum - 1) && ItemLoc.ColNum > (BackPackDisplaySize.ColNum - 1)) //Is Location within bounds
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OutOfBounds-- " + FString::FromInt(ItemLoc.RowNum) + " "
					+ FString::FromInt(ItemLoc.ColNum));
				return false;
			}
			else
			{
				if (BackPack.GetItem(ItemLoc).ItemClass == nullptr) // Is Location empty (triggered when adding item by mouse)
				{
					BackPack.GetItem(ItemLoc) = ItemStruct;
					ItemAdded(ItemStruct.ItemClass); // Items Added

					ItemStruct.EmptyItem();
				}
			}
		}
	}
	return true;
}

void APSE_LYFE_Inventory2_Storage::ItemAdded(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks)
{
	int16 ItemLocation = TotalItemsArray.GetItemClassLocation(ItemClass);
	if (ItemLocation >= 0)
	{
		uint16 TotalStacks = TotalItemsArray.ItemArray[ItemLocation].TotalStacks;
		TotalStacks += Stacks;
		TotalItemsArray.UpdateItemStacks(ItemLocation, TotalStacks);
	}
	else
	{
		TotalItemsArray.AddItemClass(ItemClass, Stacks);
		if (ItemClass->IsChildOf(APSE_LYFE_BaseWeaponMagazine::StaticClass()))
		{
			WeaponMagazineAdded(ItemClass);
		}
	}
	const float ItemWeight = CalculateWeight(ItemClass, Stacks);
	TotalItemsWeight += ItemWeight;
}

void APSE_LYFE_Inventory2_Storage::ItemRemoved(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks)
{
	int16 ItemLocation = TotalItemsArray.GetItemClassLocation(ItemClass);
	if (ItemLocation >= 0)
	{
		uint16 TotalStacks = TotalItemsArray.ItemArray[ItemLocation].TotalStacks;
		TotalStacks -= Stacks;
		TotalItemsArray.UpdateItemStacks(ItemLocation, TotalStacks);
	}
	const float ItemWeight = CalculateWeight(ItemClass, Stacks);
	TotalItemsWeight -= ItemWeight;
}

void APSE_LYFE_Inventory2_Storage::WeaponMagazineAdded(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass)
{
	const TArray<APSE_LYFE_BaseWeapon*> Weapons = OwningPawn->GetAllWeapons();
	for (int8 i = 0; i < Weapons.Num(); i++)
	{
		/*	if (Weapons[i]->IsA(APSE_LYFE_ReloadableWeapon::StaticClass()))
		{
		APSE_LYFE_ReloadableWeapon* ReloadableWeapon = Cast<APSE_LYFE_ReloadableWeapon>(Weapons[i]);
		if (ReloadableWeapon->AmmoClass == ItemClass)
		{
		TotalItemsArray.ItemArray[TotalItemsArray.ItemArray.Num() - 1].AddPointerValue(&ReloadableWeapon->NoOfClips);
		}
		else
		{
		FString String1 = "nullpptr";
		FString String2 = "nullpptr";
		if (ReloadableWeapon->AmmoClass)
		{
		String1 = ReloadableWeapon->AmmoClass->GetName();
		}
		if (ItemClass)
		{
		String2 = ItemClass->GetName();
		}
		}
		}*/
	}
}

const float APSE_LYFE_Inventory2_Storage::CalculateWeight(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass, const uint8 Stacks) const
{
	const APSE_LYFE_BaseInventoryItem* DefaultItem = ItemClass->GetDefaultObject<APSE_LYFE_BaseInventoryItem>();
	const float WeightPerItem = DefaultItem->GetItemWeight();
	return (WeightPerItem*Stacks);
}

const float APSE_LYFE_Inventory2_Storage::GetNoOfUsedSlots() const
{
	uint16 TempNoOfSlots = 0;
	for (const FItemStruct ItemStruct : BackPackBase)
	{
		if (ItemStruct.ItemClass != nullptr)
		{
			TempNoOfSlots++;
		}
	}
	return TempNoOfSlots;
}

void APSE_LYFE_Inventory2_Storage::CloseInventory()
{
	CharacterHUD->CloseInventoryUI();
	InventoryState = EInventoryState::Close;
}

void APSE_LYFE_Inventory2_Storage::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Inventory2_Storage, BackPackBase, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Inventory2_Storage, TotalItemsWeight, COND_OwnerOnly);
}
