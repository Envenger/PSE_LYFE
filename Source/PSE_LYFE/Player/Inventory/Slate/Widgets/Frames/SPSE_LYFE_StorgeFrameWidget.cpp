// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"
#include "Storage/PSE_LYFE_BaseStorage.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Player/Inventory/Slate/Widgets/Slots/SPSE_LYFE_StorageItemSlotWidget.h"
#include "SPSE_LYFE_StorgeFrameWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_StorgeFrameWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;
	ExternalStoragePtr = InArgs._ExternalStoragePtr;

	int32 InventoryRows = InventoryPtr->BackPackDisplaySize.RowNum;
	int32 InventoryCols = InventoryPtr->BackPackDisplaySize.ColNum;

	TArray<TSharedPtr<SVerticalBox>> Cols;
	for (int32 j = 0; j < InventoryCols; j++)
	{
		Cols.Add(SNew(SVerticalBox));
		for (int32 i = 0; i < InventoryRows; i++)
		{
			Cols[j]->AddSlot()
				.Padding(0.25)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SPSE_LYFE_StorageItemSlotWidget)
					.InventoryPtr(InventoryPtr)
					.ExternalStoragePtr(ExternalStoragePtr)
					.SlotLoc(FStorageLoc(i, j))
				];
		}
	}
	TSharedPtr<SHorizontalBox> Container;
	SAssignNew(Container, SHorizontalBox);
	for (int32 j = 0; j < InventoryCols; j++)
	{
		Container->AddSlot()
			.Padding(0.25)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				Cols[j].ToSharedRef()
			];
	}

	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(InventoryPtr->InventoryWidth)
			.HeightOverride((InventoryPtr->InventoryWidth)*1.375)
			[
				SNew(SBorder)
				.Padding(FMargin(37.5, 33, 38.5, 3.5))
				.BorderImage(&InventoryPtr->InventoryBorderOuter)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(0, 0, 10, 0))
						[
							SNew(STextBlock)
							.Font(InventoryPtr->InventoryHeadlineFont)
							.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
							.Justification(ETextJustify::Left)
							.Text(FString("STORAGE"))
						]
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
							[
								SNew(SImage)
								.Image(&InventoryPtr->BorderLine)
							]
						]
					]
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.AutoHeight()
					.Padding(FMargin(0, 8, 0, 0))
					[
						SNew(SBox)
						.VAlign(VAlign_Center)
						.HAlign(HAlign_Fill)
						.HeightOverride(((InventoryPtr->InventoryIconSize + (0.25 * 2)) * 5) + 1.5)
						[
							SNew(SScrollBox)
							+ SScrollBox::Slot()
							[
								Container.ToSharedRef()
							]
						]
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(FMargin(0, 12, 0, 0))
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							.HAlign(HAlign_Fill)
							.VAlign(VAlign_Center)
							[
								SNew(SImage)
								.Image(&InventoryPtr->BorderLine)
							]
						]
					]
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPSE_LYFE_StorgeFrameWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPSE_LYFE_StorgeFrameWidget::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InMyGeometry, InMouseEvent);
}