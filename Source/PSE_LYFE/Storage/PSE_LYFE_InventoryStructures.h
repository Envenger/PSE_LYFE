// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Object.h"
#include "Items/Food/PSE_LYFE_Food_Base_Stackable.h"
#include "PSE_LYFE_InventoryStructures.generated.h"

USTRUCT()
struct FStorageUnit
{
	GENERATED_USTRUCT_BODY()

	/** Which array the item is stored */
	UPROPERTY()
	TEnumAsByte<EItemType::Type> ItemType;

	/** THe index position of the array the item is stored */
	UPROPERTY()
		int32 ItemIndex;

	FStorageUnit()
	{
		ItemType = EItemType::Nothing;
		ItemIndex = -1;
	}

	FStorageUnit(TEnumAsByte<EItemType::Type> NewItemType, int32 NewItemIndex)
	{
		ItemType = NewItemType;
		ItemIndex = NewItemIndex;
	}
};

USTRUCT()
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

USTRUCT()
struct FStorageArray
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FStorageRow> Rows;

	void AddNewRow()
	{
		Rows.Add(FStorageRow());
	}

	void AddUninitialized(const int32 RowCount, const int32 ColCount)
	{
		//Add Rows
		for (int32 i = 0; i < RowCount; i++)
		{
			AddNewRow();
		}
		//Add Columns
		for (int32 i = 0; i < RowCount; i++)
		{
			for (int32 j = 0; j < ColCount; j++)
			{
				Rows[i].AddNewColumn();
			}
		}
	}
	FStorageArray()
	{}
};

/** A simple 2d int32 structure */
USTRUCT()
struct FStorageLoc
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 RowNum;

	UPROPERTY()
	int32 ColNum;

	FStorageLoc()
	{
		RowNum = -1;
		ColNum = -1;
	}

	FStorageLoc(int32 Row, int32 Col)
	{
		RowNum = Row;
		ColNum = Col;
	}
};


/** Hold the item along with its inventory location */
USTRUCT()
struct FStackableSingleFoodItemLoc
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FStackableSingleFoodItemStruct StackableSingleFoodItem;

	UPROPERTY()
	FStorageLoc ItemLoc;

	FStackableSingleFoodItemLoc()
	{}

	FStackableSingleFoodItemLoc(FStackableSingleFoodItemStruct NewStackableSingleFoodItem, FStorageLoc NewItemLoc)
	{
		StackableSingleFoodItem = NewStackableSingleFoodItem;
		ItemLoc = NewItemLoc;
	}
};

UCLASS()
class PSE_LYFE_API UPSE_LYFE_InventoryStructures : public UObject
{	GENERATED_BODY()};
