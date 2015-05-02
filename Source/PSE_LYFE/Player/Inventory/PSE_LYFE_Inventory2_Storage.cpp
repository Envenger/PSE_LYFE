// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Items/Ammo/PSE_LYFE_BaseWeaponMagazine.h"
#include "Weapons/BaseFiles/PSE_LYFE_ReloadableWeapon.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Inventory2_Storage.h"

APSE_LYFE_Inventory2_Storage::APSE_LYFE_Inventory2_Storage()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultStorageSize = 5;
	BagSize = 0;//Initializing default bag size

	InventoryWidth = 500;
	InventoryIconSize = 65;
}

void APSE_LYFE_Inventory2_Storage::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetStorageBagSize(BagSize);
}

const uint16 APSE_LYFE_Inventory2_Storage::GetTotalStorageSize() const
{
	return DefaultStorageSize + BagSize;
}

void APSE_LYFE_Inventory2_Storage::SetStorageBagSize(const int16 BagSize)
{
	uint16 TotalStorageSize = GetTotalStorageSize();
	int32 NoOfRows = FMath::CeilToInt((float)TotalStorageSize / 5);
	if (NoOfRows < 5)
	{
		NoOfRows = 5;
	}
	StorageDisplaySize = FStorageLoc(NoOfRows, 5);
	Storage.AddUninitialized(NoOfRows, 5);
	Storage.InitializeArray(StorageBase, TotalStorageSize);
}

void APSE_LYFE_Inventory2_Storage::ResetStorageSize(const int16 NewBagSize)
{
	uint16 NewTotalStorageSize = NewBagSize + DefaultStorageSize;
	uint16 OldTotalStorageSize = GetTotalStorageSize();
	int16 NoOfRows = FMath::CeilToInt((float)NewTotalStorageSize / 5);
	if (NoOfRows < 5)
	{
		NoOfRows = 5;
	}
	FStorageLoc NewStorageDisplaySize = FStorageLoc(NoOfRows, 5);
		if (NewTotalStorageSize < OldTotalStorageSize)
	{
		int16 ElementsToRemove = OldTotalStorageSize - NewTotalStorageSize;
		StorageBase.RemoveAt(NewTotalStorageSize, ElementsToRemove);
	}
	else
	{
		int16 ElementsToAdd = NewTotalStorageSize - OldTotalStorageSize;
		FItemStruct TempStruct;
		TempStruct.ItemClass = nullptr;
		for (int16 i = ElementsToAdd; i > 0; i--)
		{
			StorageBase.Add(TempStruct);
		}
	}
	FStorageArray TempStorageArray;
	TempStorageArray.StorageBasePtr = &StorageBase;
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
	Storage = TempStorageArray;
	BagSize = NewBagSize;
	StorageDisplaySize = NewStorageDisplaySize;
}

const int16 APSE_LYFE_Inventory2_Storage::GetLowestItemIndex() const
{
	for (int16 i = (StorageBase.Num() - 1); i >= 0; i--)
	{
		if (StorageBase[i].ItemClass != nullptr)
		{
			return i;
		}
	}
	return -1;
}

void APSE_LYFE_Inventory2_Storage::OnRep_ClientResetBagSize()
{

	if (StorageBase.Num() == GetTotalStorageSize())
	{
		return;
	}
	if (CharacterHUD->bIsInventoryOpen)
	{
		CharacterHUD->CloseStorageSlot();
	}
	int16 NewStorageSize = StorageBase.Num();
	
	int16 NoOfRows = FMath::CeilToInt((float)NewStorageSize / 5);
	if (NoOfRows < 5)
	{
		NoOfRows = 5;
	}
	FStorageLoc NewStorageDisplaySize = FStorageLoc(NoOfRows, 5);

	FStorageArray TempStorageArray;
	TempStorageArray.StorageBasePtr = &StorageBase;
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
	Storage = TempStorageArray;
	BagSize = NewStorageSize - DefaultStorageSize;
	StorageDisplaySize = NewStorageDisplaySize;
	if (CharacterHUD->bIsInventoryOpen)
	{
		CharacterHUD->CreateStorageSlot();
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
		if (NewBagSize != BagSize)
		{
			ResetStorageSize(NewBagSize);
		}
		//ResetStorageSize(32);
	}
}

void APSE_LYFE_Inventory2_Storage::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void APSE_LYFE_Inventory2_Storage::StorageSlotLeftClick(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !Storage.HasValidItem(ItemLoc))
	{
		return;
	}
	Server_StorageSlotLeftClick(ItemLoc);
}

bool APSE_LYFE_Inventory2_Storage::Server_StorageSlotLeftClick_Validate(const FStorageLoc ItemLoc)
{
	return true;
}

void APSE_LYFE_Inventory2_Storage::Server_StorageSlotLeftClick_Implementation(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !Storage.HasValidItem(ItemLoc))
	{
		return;
	}
	if (bIsAltPressed == false)
	{
		if (CursorItem.ItemClass == nullptr)
		{
			if (Storage.GetItem(ItemLoc).ItemClass != nullptr)
			{
				AddItemToCursor(Storage.GetItem(ItemLoc), ItemLoc);
			}
		}
		else
		{
			if (Storage.GetItem(ItemLoc).ItemClass == nullptr)
			{
				AddItem(CursorItem, ItemLoc);
			}
			else if (Storage.GetItem(ItemLoc).ItemClass == CursorItem.ItemClass)
			{
				const APSE_LYFE_BaseInventoryItem* BaseItem = CursorItem.GetDefaultItem();
				if (BaseItem->ItemType == EItemType::StackableItem || BaseItem->ItemType == EItemType::StackableUsableItem)
				{
					AddItem(CursorItem, ItemLoc);
				}
				else if (Storage.GetItem(ItemLoc).ItemProperties != CursorItem.ItemProperties)
				{
					CursorSwap(Storage.GetItem(ItemLoc), ItemLoc);
				}
			}
			else
			{
				CursorSwap(Storage.GetItem(ItemLoc), ItemLoc);
			}
		}
	}
	else
	{
		if (Storage.GetItem(ItemLoc).ItemClass != nullptr)
		{
			AddItemToCursorAlt(Storage.GetItem(ItemLoc), ItemLoc);
		}
	}
	
}

void APSE_LYFE_Inventory2_Storage::StorageSlotRightClick(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !Storage.HasValidItem(ItemLoc))
	{
		return;
	}
	Server_StorageSlotRightClick(ItemLoc);
}

bool APSE_LYFE_Inventory2_Storage::Server_StorageSlotRightClick_Validate(const FStorageLoc ItemLoc)
{
	return true;
}

void APSE_LYFE_Inventory2_Storage::Server_StorageSlotRightClick_Implementation(const FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional || !Storage.HasValidItem(ItemLoc))
	{
		return;
	}
	if (Storage.GetItem(ItemLoc).ItemClass == nullptr)
	{
		ResetItemLastLocation();
		return;
	}
	const APSE_LYFE_BaseInventoryItem* BaseItem = Storage.GetItem(ItemLoc).GetDefaultItem();
	if (BaseItem->ItemType == EItemType::UsableItem || BaseItem->ItemType == EItemType::StackableUsableItem)
	{
		BaseItem->UseItem(OwningPawn);
		DeleteItems(ItemLoc, 1); // Use Item
	}
	else if (BaseItem->ItemType == EItemType::EquipableItem)
	{
		EquipItem(ItemLoc);
	}
}

void APSE_LYFE_Inventory2_Storage::EquipItem(const FStorageLoc ItemLoc)
{	/* Should be overladed in the Inventory3 */}

const FStorageLoc APSE_LYFE_Inventory2_Storage::FindFirstEmptySlot()
{
	for (uint16 i = 0; i < StorageBase.Num(); i++)
	{
		if (StorageBase[i].ItemClass == nullptr)
		{
			return Storage.GetStorageLocFromIndex(i);
		}
	}
	return FStorageLoc(-1, -1);
}

const FStorageLoc APSE_LYFE_Inventory2_Storage::FindFirstFreeSlot(const TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass)
{
	const APSE_LYFE_BaseInventoryItem* BaseItem = ItemClass.GetDefaultObject();
	if (BaseItem->ItemType != EItemType::StackableItem && BaseItem->ItemType != EItemType::StackableUsableItem)
	{
		return FindFirstEmptySlot();
	}
	for (uint16 i = 0; i < StorageBase.Num(); i++)
	{
		if (ItemClass == StorageBase[i].ItemClass)
		{
			const int32 MaxStacks = BaseItem->MaxStacks;
			int32 CurrentStacks = StorageBase[i].ItemProperties[0];
			if (CurrentStacks < MaxStacks)
			{
				return Storage.GetStorageLocFromIndex(i);
			}
		}
	}
	return FStorageLoc(-1, -1);
}

void APSE_LYFE_Inventory2_Storage::SwapItems(const FStorageLoc Loc1, const FStorageLoc Loc2)// CUrrently not doing out of bound test(8 checks)
{
	FItemStruct TempItemStruct; //Temp values to use before moving.
	TempItemStruct = Storage.GetItem(Loc1);
	Storage.GetItem(Loc1) = Storage.GetItem(Loc2);
	Storage.GetItem(Loc2) = TempItemStruct;
}

void APSE_LYFE_Inventory2_Storage::ThrowItems(const FStorageLoc ItemLoc, const int32 Stacks)
{
	if (Storage.GetItem(ItemLoc).ItemClass == nullptr)
	{
		return;
	}/*
	const APSE_LYFE_BaseInventoryItem* ThrownItemType = Storage.GetItem(ItemLoc).GetDefaultItem();
	if (ThrownItemType->ItemType == EItemType::StackableItem || ThrownItemType->ItemType == EItemType::StackableUsableItem)
	{
		int32 &CurrentStacks = Storage.GetItem(ItemLoc).ItemProperties[0];
		if (CurrentStacks - Stacks > 0 && Stacks > -1)// Items will remain even after deletions
		{
			ItemRemoved(Storage.GetItem(ItemLoc).ItemClass, Stacks);
			CurrentStacks -= Stacks;
		}
		else // Item needs to be completely removed
		{
			ItemRemoved(Storage.GetItem(ItemLoc).ItemClass, Storage.GetItem(ItemLoc).ItemProperties[0]);
			Storage.GetItem(ItemLoc).EmptyItem();
		}
	}*/
}

void APSE_LYFE_Inventory2_Storage::DeleteItems(const FStorageLoc ItemLoc, const int32 Stacks)
{
	const APSE_LYFE_BaseInventoryItem* DeletingItemType = Storage.GetItem(ItemLoc).GetDefaultItem();
	if (DeletingItemType->ItemType == EItemType::StackableItem || DeletingItemType->ItemType == EItemType::StackableUsableItem)
	{
		int32 &CurrentStacks = Storage.GetItem(ItemLoc).ItemProperties[0];
		if (CurrentStacks - Stacks > 0 && Stacks > -1)// Items will remain even after deletions
		{
			ItemRemoved(Storage.GetItem(ItemLoc).ItemClass, Stacks);
			CurrentStacks -= Stacks;
		}
		else // Item needs to be completely removed
		{
			ItemRemoved(Storage.GetItem(ItemLoc).ItemClass, Storage.GetItem(ItemLoc).ItemProperties[0]);
			Storage.GetItem(ItemLoc).EmptyItem();
		}
	}
	else if (Stacks != 0)
	{
		ItemRemoved(Storage.GetItem(ItemLoc).ItemClass);
		Storage.GetItem(ItemLoc).EmptyItem();
	}
}

const bool APSE_LYFE_Inventory2_Storage::AddItem(FItemStruct &ItemStruct, const FStorageLoc ItemLoc)
{
	if (ItemStruct.ItemClass == nullptr)
	{
		return false;
	}
	const APSE_LYFE_BaseInventoryItem* AddedItem = ItemStruct.GetDefaultItem();
	if (AddedItem->ItemType == EItemType::StackableItem || AddedItem->ItemType == EItemType::StackableUsableItem)
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
				const int32 MaxStacks = AddedItem->MaxStacks;
				int32 &StoredCurrentStacks = Storage.GetItem(FirstFreeItemLoc).ItemProperties[0];
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
					const int32 MaxStacks = AddedItem->MaxStacks;
					int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
					const int32 ItemsToAdd = FMath::Min(MaxStacks, AddedCurrentStacks);

					Storage.GetItem(FirstEmptyItemLoc) = ItemStruct;
					Storage.GetItem(FirstEmptyItemLoc).ItemProperties[0] = ItemsToAdd;
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
			if (ItemLoc.RowNum > (StorageDisplaySize.RowNum - 1) && ItemLoc.ColNum > (StorageDisplaySize.ColNum - 1)) //Is Location within bounds
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OutOfBounds-- " + FString::FromInt(ItemLoc.RowNum) + " "
					+ FString::FromInt(ItemLoc.ColNum));
				return false;
			}
			else
			{
				if (Storage.GetItem(ItemLoc).ItemClass != nullptr) // Is Location empty (triggered when adding item by mouse)
				{
					if (Storage.GetItem(ItemLoc).ItemClass == ItemStruct.ItemClass)
					{
						const int32 MaxStacks = AddedItem->MaxStacks;
						int32 &StoredCurrentStacks = Storage.GetItem(ItemLoc).ItemProperties[0];
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
					const int32 MaxStacks = AddedItem->MaxStacks;
					int32 &AddedCurrentStacks = ItemStruct.ItemProperties[0];
					const int32 ItemsToAdd = FMath::Min(MaxStacks, AddedCurrentStacks);

					Storage.GetItem(ItemLoc) = ItemStruct;
					Storage.GetItem(ItemLoc).ItemProperties[0] = ItemsToAdd;
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
				Storage.GetItem(FirstEmptyItemLoc) = ItemStruct;
				ItemAdded(ItemStruct.ItemClass); // Items Added

				ItemStruct.EmptyItem();
			}
		}
		else
		{
			if (ItemLoc.RowNum > (StorageDisplaySize.RowNum - 1) && ItemLoc.ColNum > (StorageDisplaySize.ColNum - 1)) //Is Location within bounds
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OutOfBounds-- " + FString::FromInt(ItemLoc.RowNum) + " "
					+ FString::FromInt(ItemLoc.ColNum));
				return false;
			}
			else
			{
				if (Storage.GetItem(ItemLoc).ItemClass == nullptr) // Is Location empty (triggered when adding item by mouse)
				{
					Storage.GetItem(ItemLoc) = ItemStruct;
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
			TArray<APSE_LYFE_BaseWeapon*> Weapons = OwningPawn->GetAllWeapons();
			for (int8 i = 0; i < Weapons.Num(); i++)
			{
				if (Weapons[i]->IsA(APSE_LYFE_ReloadableWeapon::StaticClass()))
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
				}
			}
		}
	}
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
}

void APSE_LYFE_Inventory2_Storage::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Inventory2_Storage, StorageBase, COND_OwnerOnly);
}
