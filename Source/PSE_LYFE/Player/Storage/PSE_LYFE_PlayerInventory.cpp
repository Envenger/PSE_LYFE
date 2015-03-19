// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/PSE_LYFE_Character.h"
#include "Items/PSE_LYFE_Item_Base.h"
#include "Items/Food/PSE_LYFE_Food_Base_Stackable.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_PlayerInventory.h"

APSE_LYFE_PlayerInventory::APSE_LYFE_PlayerInventory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
	bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;

	PrimaryActorTick.bCanEverTick = true;
	StorageMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("StorageMesh"));
	RootComponent = StorageMesh;

	StorageSize = FStorageLoc(4, 3);//Initializing defualt bag size
	bIsCursorFunctional = true;
}

void APSE_LYFE_PlayerInventory::SetOwningPawn(APSE_LYFE_Character* Owner)
{
	InventoryOwner = Owner;
	Instigator = Owner;
	SetOwner(Owner);
}

void APSE_LYFE_PlayerInventory::BeginPlay()
{
	SetBagSize(StorageSize);
}

void APSE_LYFE_PlayerInventory::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//UpdateCursorItem();
	if (InventoryOwner)
	{
		if (InventoryOwner->IsLocallyControlled())
		{
			for (int32 i = 0; i < StackableSingleFoodItemArray.Num(); i++)
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Green, StackableSingleFoodItemArray[i].StackableSingleFoodItem.ItemClass->GetName() + " " + FString::FromInt(StackableSingleFoodItemArray[i].StackableSingleFoodItem.CurrentStacks) + " Stacks");
			}
			
		}
	}
}

void APSE_LYFE_PlayerInventory::LeftClick(FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional)
	{
		return;
	}
	if (CursorItemType == EItemType::Nothing)
	{
		if (Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType != EItemType::Nothing)
		{
			AddItemToCursor(ItemLoc);
		}
	}
	else
	{
		if (Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType == EItemType::Nothing)
		{
			AddItem(StackableSingleFoodItemCursor, ItemLoc);
		}
		else if (CursorItemType == EItemType::StackableSingleConsumable && Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType == EItemType::StackableSingleConsumable)
		{
			int32 CheckIndex = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex;
			if (StackableSingleFoodItemCursor.ItemClass == StackableSingleFoodItemArray[CheckIndex].StackableSingleFoodItem.ItemClass)
			{
				AddItem(StackableSingleFoodItemCursor, ItemLoc);
			}
			else
			{
				CursorSwap(ItemLoc);
			}
		}
		else
		{
			CursorSwap(ItemLoc);
		}
	}
}

void APSE_LYFE_PlayerInventory::AddItemToCursor(FStorageLoc ItemLoc)
{
	int32 ItemIndex = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex;
	if (Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType == EItemType::StackableSingleConsumable)
	{
		CursorItemType = EItemType::StackableSingleConsumable;
		StackableSingleFoodItemCursor = StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem;
		int32 ItemsToAdd = FMath::Min(StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem.MaxStacks, StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem.CurrentStacks);
		StackableSingleFoodItemCursor.CurrentStacks = ItemsToAdd;
		DeleteItems(ItemLoc, ItemsToAdd);
	}
	LastStoredLoc = ItemLoc;
}

void APSE_LYFE_PlayerInventory::CursorSwap(FStorageLoc ItemLoc)
{
	if (Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType == EItemType::StackableSingleConsumable)
	{
		FStackableSingleFoodItemStruct TempStackableSingleFoodItemCursor;
		TempStackableSingleFoodItemCursor = StackableSingleFoodItemCursor;
		AddItemToCursor(ItemLoc);
		AddItem(TempStackableSingleFoodItemCursor, ItemLoc);
	}
	//////////
}
/*
void APSE_LYFE_PlayerInventory::T3st1()
{
	//FStorageLoc Taba(FMath::RandRange(0, (StorageSize.RowNum - 1)), FMath::RandRange(0, (StorageSize.ColNum - 1)));
	TestStackableFoodItem1.CurrentStacks = 7;
	AddItem(TestStackableFoodItem1);
	TestStackableFoodItem1.CurrentStacks = 3;
	AddItem(TestStackableFoodItem1, FStorageLoc(2, 1));
	TestStackableFoodItem1.CurrentStacks = 15;
	AddItem(TestStackableFoodItem2);
	TestStackableFoodItem2.CurrentStacks = 3;
	AddItem(TestStackableFoodItem2, FStorageLoc(2, 0));
}*/



void APSE_LYFE_PlayerInventory::SetBagSize(FStorageLoc BagSize)
{
	Storage.AddUninitialized(BagSize.RowNum, BagSize.ColNum);// Values are swaped to keep the index in order
}

FStorageLoc APSE_LYFE_PlayerInventory::FindFirstEmptySlot()
{
	for (int32 i = 0; i < Storage.Rows.Num(); i++)
	{
		for (int32 j = 0; j < Storage.Rows[i].Columns.Num(); j++)
		{
			if (Storage.Rows[i].Columns[j].ItemType == EItemType::Nothing)
			{
				return FStorageLoc(i, j);
			}
		}
	}
	return FStorageLoc(-1, -1);
}

int32 APSE_LYFE_PlayerInventory::FindFirstFreeSlot(TSubclassOf<class APSE_LYFE_Item_Base> ItemClass)
{
	for (int32 i = 0; i < Storage.Rows.Num(); i++)
	{
		for (int32 j = 0; j < Storage.Rows[i].Columns.Num(); j++)
		{
			TEnumAsByte<EItemType::Type> SearchItemType = ItemClass->StaticClass()->GetDefaultObject<APSE_LYFE_Item_Base>()->ItemType;
			if (SearchItemType == Storage.Rows[i].Columns[j].ItemType)
			{
				int32 ItemIndex = Storage.Rows[i].Columns[j].ItemIndex;
				if (StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem.ItemClass == ItemClass)
				{
					if (StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem.CurrentStacks < StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem.MaxStacks)
					{
						return ItemIndex;
					}
				}
			}
		}
	}
	return -1;
}

void APSE_LYFE_PlayerInventory::UseItem(FStorageLoc ItemLoc)
{
	if (!bIsCursorFunctional)
	{
		return;
	}
	TEnumAsByte<EItemType::Type> ItemType = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType;
	if (ItemType == EItemType::StackableSingleConsumable)
	{
		int32 Index = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex;
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green,
			StackableSingleFoodItemArray[Index].StackableSingleFoodItem.ItemClass->GetName()
			+ " Item Used");
		if (StackableSingleFoodItemArray[Index].StackableSingleFoodItem.CurrentStacks > 1)// Items will remain even after deletions
		{
			StackableSingleFoodItemArray[Index].StackableSingleFoodItem.CurrentStacks -= 1;
		}
		else // Item needs to be completly removed
		{
			int32 ItemArrayIndex = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex;
			Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex = -1;
			Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType = EItemType::Nothing;
			StackableSingleFoodItemArray.RemoveAt(ItemArrayIndex);
			for (int32 i = ItemArrayIndex; i < StackableSingleFoodItemArray.Num(); i++)
			{
				FStorageLoc ItemLoc = StackableSingleFoodItemArray[i].ItemLoc;
				Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex = i;
			}
		}
	}
}

void APSE_LYFE_PlayerInventory::DeleteItems(FStorageLoc ItemLoc, int32 Stacks)
{
	TEnumAsByte<EItemType::Type> ItemType = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType;
	if (ItemType == EItemType::StackableSingleConsumable)
	{
		int32 Index = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex;
		if (StackableSingleFoodItemArray[Index].StackableSingleFoodItem.CurrentStacks - Stacks > 0 && Stacks > -1)// Items will remain even after deletions
		{
			StackableSingleFoodItemArray[Index].StackableSingleFoodItem.CurrentStacks -= Stacks;
		}
		else // Item needs to be completly removed
		{
			int32 ItemArrayIndex = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex;
			Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex = -1;
			Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType = EItemType::Nothing;
			StackableSingleFoodItemArray.RemoveAt(ItemArrayIndex);
			for (int32 i = ItemArrayIndex; i < StackableSingleFoodItemArray.Num(); i++)
			{
				FStorageLoc ItemLoc = StackableSingleFoodItemArray[i].ItemLoc;
				Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex = i;
			}
		}
	}
}

void APSE_LYFE_PlayerInventory::AddItem(FStackableSingleFoodItemStruct &StackableSingleFoodItem, FStorageLoc ItemLoc)
{
	if (StackableSingleFoodItem.CurrentStacks <= 0)
	{
		return;
	}
	if (ItemLoc.RowNum <= -1 || ItemLoc.ColNum <= -1)// No particular slots are allocated for the item storage
	{
		int32 InternalItemLocation = FindFirstFreeSlot(StackableSingleFoodItem.ItemClass);
		if (InternalItemLocation > -1)// Item is found and slot is not full
		{
			int32 MaxStacks = StackableSingleFoodItemArray[InternalItemLocation].StackableSingleFoodItem.MaxStacks;
			int32 CurrentStacks = StackableSingleFoodItemArray[InternalItemLocation].StackableSingleFoodItem.CurrentStacks;
			int32 ItemsToAdd = FMath::Min((MaxStacks - CurrentStacks), StackableSingleFoodItem.CurrentStacks);// Checks how many items are needed to reach the top of the stack
			StackableSingleFoodItemArray[InternalItemLocation].StackableSingleFoodItem.CurrentStacks += ItemsToAdd; //Adds item to the exsisting slot
			StackableSingleFoodItem.CurrentStacks -= ItemsToAdd; // Removes Items from the Original Place
			if (StackableSingleFoodItem.CurrentStacks > 0)// Checks there are any items left to add
			{
				AddItem(StackableSingleFoodItem); // If there are items the re-loop to add them.
			}
		}
		else // Item will be stored in an completly empty slot(Item not found or slot full)
		{
			ItemLoc = FindFirstEmptySlot();
			if (ItemLoc.RowNum <= -1 && ItemLoc.ColNum <= -1)
			{
				GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Inventory full");
			}
			else
			{
				int32 ItemsToAdd = FMath::Min(StackableSingleFoodItem.MaxStacks, StackableSingleFoodItem.CurrentStacks);
				FStackableSingleFoodItemStruct TempStackableSingleFoodItem = StackableSingleFoodItem;// The stacks to be added for now
				TempStackableSingleFoodItem.CurrentStacks = ItemsToAdd; //New Slot Items
				StackableSingleFoodItem.CurrentStacks -= ItemsToAdd; //Items removed from exsisting

				StackableSingleFoodItemArray.Add(FStackableSingleFoodItemLoc(TempStackableSingleFoodItem, ItemLoc));
				int32 Index = StackableSingleFoodItemArray.Num() - 1;
				Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex = Index;
				Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType = EItemType::StackableSingleConsumable;
				if (StackableSingleFoodItem.CurrentStacks > 0)// Stacks to pich is more than max stacks
				{
					AddItem(StackableSingleFoodItem);
				}
			}
		}
	}
	else
	{
		if (ItemLoc.RowNum > (Storage.Rows.Num() - 1) && ItemLoc.ColNum > (Storage.Rows.Num() - 1))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "OutOfBounds");
		}
		else
		{
			if (Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType != EItemType::Nothing)
			{
				int32 CheckIndex = Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex;
				if (StackableSingleFoodItem.ItemClass == StackableSingleFoodItemArray[CheckIndex].StackableSingleFoodItem.ItemClass)
				{
					if (StackableSingleFoodItemArray[CheckIndex].StackableSingleFoodItem.CurrentStacks < StackableSingleFoodItemArray[CheckIndex].StackableSingleFoodItem.MaxStacks)
					{
						int32 ItemsToAdd = FMath::Min(StackableSingleFoodItem.CurrentStacks, (StackableSingleFoodItemArray[CheckIndex].StackableSingleFoodItem.MaxStacks - StackableSingleFoodItemArray[CheckIndex].StackableSingleFoodItem.CurrentStacks));
						StackableSingleFoodItem.CurrentStacks -= ItemsToAdd;
						StackableSingleFoodItemArray[CheckIndex].StackableSingleFoodItem.CurrentStacks += ItemsToAdd;
					}
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Class No compatible");
					AddItem(StackableSingleFoodItem);
				}
			}
			else// Slot empty
			{
				int32 ItemsToAdd = FMath::Min(StackableSingleFoodItem.MaxStacks, StackableSingleFoodItem.CurrentStacks);
				FStackableSingleFoodItemStruct TempStackableSingleFoodItem = StackableSingleFoodItem;// The stacks to be added for now
				TempStackableSingleFoodItem.CurrentStacks = ItemsToAdd; //New Slot Items
				StackableSingleFoodItem.CurrentStacks -= ItemsToAdd;

				StackableSingleFoodItemArray.Add(FStackableSingleFoodItemLoc(TempStackableSingleFoodItem, ItemLoc));
				int32 Index = StackableSingleFoodItemArray.Num() - 1;
				Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemIndex = Index;
				Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].ItemType = EItemType::StackableSingleConsumable;
			}
		}
	}
}

void APSE_LYFE_PlayerInventory::UpdateCursorItem()
{
	if (CursorItemType == EItemType::StackableSingleConsumable)
	{
		if (StackableSingleFoodItemCursor.CurrentStacks == 0)
		{
			CursorItemType = EItemType::Nothing;
		}
	}
}

void APSE_LYFE_PlayerInventory::ThrowAllCursorItems()
{
	if (CursorItemType != EItemType::Nothing)
	{
		GetWorldTimerManager().SetTimer(this, &APSE_LYFE_PlayerInventory::ThrowAllCursorItems, 0.15, false);
		ThrowCursorItem();
	}
	else
	{
		bIsCursorFunctional = true;
	}
}

void APSE_LYFE_PlayerInventory::ThrowCursorItem()
{
	if (CursorItemType == EItemType::StackableSingleConsumable)
	{
		if (StackableSingleFoodItemCursor.CurrentStacks > 0)
		{
			if (StackableSingleFoodItemCursor.CurrentStacks == 1)
			{
				CursorItemType = EItemType::Nothing;
			}
			else
			{
				StackableSingleFoodItemCursor.CurrentStacks -= 1;
			}
			StackableSingleFoodItemCursor.CreateItem(GetWorld(), FVector(600, 0, 270));
		}
	}
}



void APSE_LYFE_PlayerInventory::AttemptPickItem(APSE_LYFE_Item_Base* PickedItem)
{
	if (PickedItem->IsA(APSE_LYFE_Food_Base_Stackable::StaticClass()))
	{
		APSE_LYFE_Food_Base_Stackable* TempStackableFoodItem = Cast<APSE_LYFE_Food_Base_Stackable>(PickedItem);
		FStackableSingleFoodItemStruct TempFoodItemStruct = TempStackableFoodItem->CreateStruct();
		AddItem(TempFoodItemStruct);
	}
	PickedItem->Destroy();
}

void APSE_LYFE_PlayerInventory::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_PlayerInventory, Storage, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_PlayerInventory, StackableSingleFoodItemArray, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_PlayerInventory, CursorItemType, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_PlayerInventory, StackableSingleFoodItemCursor, COND_OwnerOnly);
	DOREPLIFETIME(APSE_LYFE_PlayerInventory, InventoryOwner);
}