// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "PSE_LYFE_InventoryStructures.generated.h"

/** A simple 2d int32 structure */
USTRUCT()
struct FStorageLoc
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int8 RowNum;

	UPROPERTY()
	int8 ColNum;

	FStorageLoc()
	{
		RowNum = 0;
		ColNum = 0;
	}

	FStorageLoc(int8 Row, int8 Col)
	{
		RowNum = Row;
		ColNum = Col;
	}
};


USTRUCT(BlueprintType)
struct FStorageUnit
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int16 Index;

	FStorageUnit()
	{
		Index = -1;
	}

	FStorageUnit(int16 NewIndex)
	{
		Index = NewIndex;
		//ItemStruct = NewItemStruct;
	}
};

USTRUCT(BlueprintType)
struct FStorageRow
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FStorageUnit> Columns;

	void AddNewColumn()
	{
		Columns.Add(FStorageUnit());
	}
	//default properties
	FStorageRow(){}
};

USTRUCT(BlueprintType)
struct FStorageArray
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FStorageRow> Rows;

	TArray<FItemStruct>* StorageBasePtr;

	void AddNewRow()
	{
		Rows.Add(FStorageRow());
	}

	/** No of rows and colos need to reversed */
	void AddUninitialized(const int8 RowCount, const int8 ColCount)
	{
		//Add Rows
		for (int32 i = 0; i < RowCount; i++)
		{
			AddNewRow();
		}
		//Add Columns
		for (int8 i = 0; i < RowCount; i++)
		{
			for (int8 j = 0; j < ColCount; j++)
			{
				Rows[i].AddNewColumn();
			}
		}
	}

	bool InitializeArray(TArray<FItemStruct> &StorageArray, const uint16 StorageSize)
	{
		if (Rows.Num() > 0 && Rows[0].Columns.Num() > 0)
		{
			StorageBasePtr = &StorageArray;
			FItemStruct TempStruct;
			TempStruct.ItemClass = nullptr;
			uint16 SlotsAssigned = 0;
			for (int8 i = 0; i < Rows.Num(); i++)
			{
				for (int8 j = 0; j < Rows[i].Columns.Num(); j++)
				{
					if (SlotsAssigned < StorageSize)
					{
						StorageArray.Add(TempStruct);
						Rows[i].Columns[j].Index = (StorageArray.Num() - 1);
						SlotsAssigned++;
					}
					else
					{
						Rows[i].Columns[j].Index = -1;
					}
				}
			}
			return true;
		}
		return false;
	}

	FItemStruct& GetItem(FStorageLoc ItemLocation)
	{
		const int16 Index = Rows[ItemLocation.RowNum].Columns[ItemLocation.ColNum].Index;
		TArray<FItemStruct>& ReferenceArray = *StorageBasePtr;
		return ReferenceArray[Index];
	}

	const FStorageLoc GetStorageLocFromIndex(uint16 Index)
	{
		if (Index < StorageBasePtr->Num())
		{
			const uint8 NoOfRows = FMath::FloorToInt((float)Index / Rows[0].Columns.Num());
			const uint8 NoOfCols = Index - (NoOfRows * Rows[0].Columns.Num());
			return FStorageLoc(NoOfRows, NoOfCols);
		}
		return FStorageLoc(-1, -1);
	}

	FStorageArray()
	{}
};


USTRUCT(BlueprintType)
struct FTotalItemUnitStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass;

	UPROPERTY()
	uint16 TotalStacks;

	TArray<uint16*> HoldingValues;

	void AddPointerValue(uint16* NewPointerLocation)
	{

		*NewPointerLocation = TotalStacks;
		HoldingValues.AddUnique(NewPointerLocation);
	}

	//default properties
	FTotalItemUnitStruct()
	{
		ItemClass = nullptr;
		TotalStacks = 0;
	}

	FTotalItemUnitStruct(TSubclassOf<class APSE_LYFE_BaseInventoryItem> NewItemClass, const uint16 NewTotalStacks)
	{
		ItemClass = NewItemClass;
		TotalStacks = NewTotalStacks;
	}
};

USTRUCT(BlueprintType)
struct FTotalItemStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FTotalItemUnitStruct> ItemArray;

	const int16 GetItemClassLocation(TSubclassOf<class APSE_LYFE_BaseInventoryItem> CheckItemClass) const
	{
		if (CheckItemClass != nullptr)
		{
			for (uint16 i = 0; i < ItemArray.Num(); i++)
			{
				if (ItemArray[i].ItemClass == CheckItemClass)
				{
					return i;
				}
			}
		}
		return -1;
	}

	const uint16 GetTotalItemsPresent(TSubclassOf<class APSE_LYFE_BaseInventoryItem> CheckItemClass) const
	{
		const int16 ItemPosition = GetItemClassLocation(CheckItemClass);
		if (ItemPosition >= 0)
		{
			return ItemArray[ItemPosition].TotalStacks;
		}
		return 0;
	}

	void AddItemClass(TSubclassOf<class APSE_LYFE_BaseInventoryItem> CheckItemClass, const uint16 Stacks)
	{
		FTotalItemUnitStruct TempTotalItemUnit(CheckItemClass, Stacks);
		ItemArray.Add(TempTotalItemUnit);
	}

	void UpdateItemStacks(const uint16 ItemLocation, const uint16 NewItemStacks)
	{
		if (ItemArray.IsValidIndex(ItemLocation))
		{
			if (NewItemStacks > 0)
			{
				ItemArray[ItemLocation].TotalStacks = NewItemStacks;
				for (int32 i = 0; i < ItemArray[ItemLocation].HoldingValues.Num(); i++)
				{
					*ItemArray[ItemLocation].HoldingValues[i] = NewItemStacks;
				}
			}
			else
			{
				for (int32 i = 0; i < ItemArray[ItemLocation].HoldingValues.Num(); i++)
				{
					*ItemArray[ItemLocation].HoldingValues[i] = 0;
				}
				ItemArray.RemoveAtSwap(ItemLocation);
			}
		}
	}



	FTotalItemStruct()
	{

	}
};

UCLASS()
class PSE_LYFE_API UPSE_LYFE_InventoryStructures : public UObject
{
	GENERATED_BODY()
};
