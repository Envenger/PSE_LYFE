// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "Player/Storage/PSE_LYFE_PlayerInventory.h"
#include "SlateBasics.h"
#include "STextBlock.h"
#include "SBorder.h"
#include "SImage.h"
#include "SItemWidget.h"
#include "SPlayerInventoryWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPlayerInventoryWidget::Construct(const FArguments& InArgs)
{
	InvStorage = TWeakObjectPtr<APSE_LYFE_PlayerInventory>(InArgs._InvStorage);

	int32 InventoryRows = InvStorage->StorageSize.RowNum;
	int32 InventoryCols = InvStorage->StorageSize.ColNum;

	TArray<TSharedPtr<SVerticalBox>> Cols;
	for (int32 j = 0; j < InventoryCols; j++)
	{
		Cols.Add(SNew(SVerticalBox));
		for (int32 i = 0; i < InventoryRows; i++)
		{
			Cols[j]->AddSlot()
				.Padding(5)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SItemWidget)
					.InvStorage(InvStorage)
					.SlotLoc(FStorageLoc(i, j))
				];
		}
	}

	TSharedPtr<SHorizontalBox> Container;
	SAssignNew(Container, SHorizontalBox);
	for (int32 j = 0; j < InventoryCols; j++)
	{
		Container->AddSlot()
			.Padding(5)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				Cols[j].ToSharedRef()
			];
	}


	ChildSlot
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SAssignNew(InventoryBox, SBorder)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Center)
				.Padding(FMargin(18, 18))
				.BorderImage(&InvStorage->InventoryBorder)
				[
					Container.ToSharedRef()
				]
			]
		];

}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
