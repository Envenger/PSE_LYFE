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

UCLASS()
class PSE_LYFE_API UPSE_LYFE_InventoryStructures : public UObject
{
	GENERATED_BODY()
};
