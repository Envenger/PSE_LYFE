// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "Items/BackPack/PSE_LYFE_BaseBackPackItem.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Inventory3_Equipment.h"

APSE_LYFE_Inventory3_Equipment::APSE_LYFE_Inventory3_Equipment()
{
	EquipmentSlots.AddUninitialized(8);
	EquipmentSlots[0] = EEquipmentSlotType::Primary;
	EquipmentSlots[1] = EEquipmentSlotType::Secondary;
	EquipmentSlots[2] = EEquipmentSlotType::Melee;
	EquipmentSlots[3] = EEquipmentSlotType::Backpack;
	EquipmentSlots[4] = EEquipmentSlotType::Top;
	EquipmentSlots[5] = EEquipmentSlotType::Bottom;
	EquipmentSlots[6] = EEquipmentSlotType::Boots;
	EquipmentSlots[7] = EEquipmentSlotType::Gloves;

	DisplayActorSpawnLocation = FVector(198.548828, 10404.232422, -16.971039);
	DisplayActorSpawnRotation = FRotator(0, 145, 0);

	bIsDisplayRotateActive = false;
	RotationCoefficent = 5;


}

void APSE_LYFE_Inventory3_Equipment::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	InitializeStorage();
}

void APSE_LYFE_Inventory3_Equipment::BeginPlay()
{
	Super::BeginPlay();
	DisplayActor = GetWorld()->SpawnActor<AActor>(DislayActorClass, DisplayActorSpawnLocation, DisplayActorSpawnRotation);
}

void APSE_LYFE_Inventory3_Equipment::InitializeStorage()
{
	FItemStruct DefaultItemStruct;
	for (int32 i = 0; i < EquipmentSlots.Num(); i++)
	{
		EquipmentStorage.Add(DefaultItemStruct);
		if (Role != ROLE_Authority)
		{
			ClientEquipmentStorage.Add(DefaultItemStruct);
		}
	}
}

bool APSE_LYFE_Inventory3_Equipment::CheckSlotType(const uint8 EquipmentSlotLoc, const EEquipmentSlotType EquipmentSlotType) const
{
	if (EquipmentStorage.IsValidIndex(EquipmentSlotLoc))
	{
		if (EquipmentSlotType == EquipmentSlots[EquipmentSlotLoc])
		{
			return true;
		}
	}
	return false;
}

void APSE_LYFE_Inventory3_Equipment::AddInventoryEquipment(FItemStruct &NewItem, const uint8 EquipmentSlotLoc)
{
	const APSE_LYFE_BaseInventoryItem* GetDefault = NewItem.GetDefaultItem();
	if (EquipmentStorage[EquipmentSlotLoc].ItemClass == nullptr)
	{
		EquipmentStorage[EquipmentSlotLoc] = NewItem;
		ItemAdded(EquipmentSlotLoc);

		NewItem.EmptyItem();
	}
	else
	{
		FItemStruct RemovedItem = EquipmentStorage[EquipmentSlotLoc];
		RemoveInventoryEquipment(EquipmentSlotLoc);

		EquipmentStorage[EquipmentSlotLoc] = NewItem;
		ItemAdded(EquipmentSlotLoc);
		NewItem = RemovedItem;
	}
}

void APSE_LYFE_Inventory3_Equipment::RemoveInventoryEquipment(const uint8 EquipmentSlotLoc)
{
	if (EquipmentStorage[EquipmentSlotLoc].ItemClass != nullptr)
	{
		ItemRemoved(EquipmentSlotLoc);
		EquipmentStorage[EquipmentSlotLoc].EmptyItem();
	}
}

void APSE_LYFE_Inventory3_Equipment::EquipItem(const FStorageLoc ItemLoc)// Replace by switch case for better efficiency and cleaner code
{
	const APSE_LYFE_BaseInventoryItem* BaseItem = Storage.GetItem(ItemLoc).GetDefaultItem();

	for (uint8 i = 0; i < EquipmentSlots.Num(); i++)
	{
		if (CheckSlotType(i, BaseItem->EquipmentSlotType))
		{
			if (EquipmentSlots[i] == EEquipmentSlotType::Backpack)
			{
				AddBackPack(Storage.GetItem(ItemLoc));
			}
			else
			{
				AddInventoryEquipment(Storage.GetItem(ItemLoc), i);
				break;
			}
		}
	}
}

void APSE_LYFE_Inventory3_Equipment::EquipmentSlotLeftClick(const uint8 SlotLoc)
{
	Server_EquipmentSlotLeftClick(SlotLoc);
}

bool APSE_LYFE_Inventory3_Equipment::Server_EquipmentSlotLeftClick_Validate(const uint8 SlotLoc)
{
	return true;
}

void APSE_LYFE_Inventory3_Equipment::Server_EquipmentSlotLeftClick_Implementation(const uint8 SlotLoc)
{
	if (CursorItem.ItemClass == nullptr)
	{
		if (EquipmentStorage[SlotLoc].ItemClass != nullptr)
		{
			if (EquipmentSlots[SlotLoc] == EEquipmentSlotType::Backpack)
			{
				RemoveBackPack();
			}
			else
			{
				AddItemToCursor(EquipmentStorage[SlotLoc], SlotLoc);
			}
		}
	}
	else
	{
		const APSE_LYFE_BaseInventoryItem* DefaultCursorItem = CursorItem.GetDefaultItem();
		if (CheckSlotType(SlotLoc, DefaultCursorItem->EquipmentSlotType))
		{
			if (EquipmentStorage[SlotLoc].ItemClass == nullptr)
			{
				if (EquipmentSlots[SlotLoc] == EEquipmentSlotType::Backpack)
				{
					AddBackPack(CursorItem);
				}
				else
				{
					AddInventoryEquipment(CursorItem, SlotLoc);
				}
			}
			else
			{
				if (EquipmentSlots[SlotLoc] == EEquipmentSlotType::Backpack)
				{
					AddBackPack(CursorItem);
				}
				else
				{
					AddInventoryEquipment(CursorItem, SlotLoc);
				}
			}
		}
	}
}

const bool APSE_LYFE_Inventory3_Equipment::AddBackPack(FItemStruct &NewBackPackItemStruct)
{
	if (!NewBackPackItemStruct.GetDefaultItem()->IsA(APSE_LYFE_BaseBackPackItem::StaticClass()))
	{
		return false;
	}
	const APSE_LYFE_BaseBackPackItem* NewBackPack = Cast<APSE_LYFE_BaseBackPackItem>(NewBackPackItemStruct.GetDefaultItem());
	uint16 NewBackPackSize = NewBackPack->BackpackSize;
	if (EquipmentStorage[3].ItemClass == nullptr)
	{
		ResetStorageSize(NewBackPackSize);
		AddInventoryEquipment(NewBackPackItemStruct, 3);
	}
	else
	{
		if (GetLowestItemIndex() < NewBackPackSize + DefaultStorageSize)
		{
			ResetStorageSize(NewBackPackSize);
			AddInventoryEquipment(NewBackPackItemStruct, 3);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "item drop error");
			// Items may drop
		}
	}
	return true;
}

const bool APSE_LYFE_Inventory3_Equipment::RemoveBackPack()
{
	if (EquipmentStorage[3].GetDefaultItem()->IsA(APSE_LYFE_BaseBackPackItem::StaticClass()))
	{
		if (GetLowestItemIndex() < DefaultStorageSize)
		{
			ResetStorageSize(0);
			AddItemToCursor(EquipmentStorage[3], 3);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, "item drop error");
			return false;
		}
	}
	return true;
}

void APSE_LYFE_Inventory3_Equipment::EquipmentSlotRightClick(const uint8 SlotLoc)
{
	//Server_EquipmentSlotRightClick(SlotLoc);
}

bool APSE_LYFE_Inventory3_Equipment::Server_EquipmentSlotRightClick_Validate(const uint8 SlotLoc)
{
	return true;
}

void APSE_LYFE_Inventory3_Equipment::Server_EquipmentSlotRightClick_Implementation(const uint8 SlotLoc)
{

}

void APSE_LYFE_Inventory3_Equipment::StartDisplayActorRotate()
{
	if (bIsCursorFunctional == true && bIsDisplayRotateActive == false)
	{
		CharacterHUD->StartDisplayActorRotate();
		bIsCursorFunctional = false;
		bIsDisplayRotateActive = true;
	}
}

void APSE_LYFE_Inventory3_Equipment::EndDisplayActorRotate()
{
	if (bIsDisplayRotateActive == true)
	{
		CharacterHUD->EndDisplayActorRotate();
		bIsCursorFunctional = true;
		bIsDisplayRotateActive = false;
	}
}

void APSE_LYFE_Inventory3_Equipment::RotateDisplayActor(float ScreenChange)
{
	if (DisplayActor)
	{
		DisplayActor->AddActorLocalRotation(FRotator(0, ScreenChange*RotationCoefficent, 0));
	}
}

void APSE_LYFE_Inventory3_Equipment::ItemAdded(const uint8 EquipmentSlotLoc)
{

}

void APSE_LYFE_Inventory3_Equipment::ItemRemoved(const uint8 EquipmentSlotLoc)
{

}

void APSE_LYFE_Inventory3_Equipment::OnRep_EquipmentChanged()
{
	if (!OwningPawn)
	{
		return;
	}
	for (uint8 i = 0; i < EquipmentStorage.Num(); i++)
	{
		if (EquipmentStorage[i] != ClientEquipmentStorage[i])
		{
			if (EquipmentStorage[i].ItemClass != nullptr)
			{
				ClientEquipmentStorage[i] = EquipmentStorage[i];
				OwningPawn->EquipItem(ClientEquipmentStorage[i]);
			}
			else
			{
				ClientEquipmentStorage[i] = EquipmentStorage[i];
				OwningPawn->UnEquipItem(EquipmentSlots[i]);
			}
		}
	}
}

void APSE_LYFE_Inventory3_Equipment::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APSE_LYFE_Inventory3_Equipment, EquipmentStorage);
}