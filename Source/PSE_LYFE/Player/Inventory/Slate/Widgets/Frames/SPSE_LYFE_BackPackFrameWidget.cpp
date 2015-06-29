// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Player/Inventory/Slate/Widgets/Slots/SPSE_LYFE_ItemSlotWidget.h"
#include "SPSE_LYFE_BackPackFrameWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_BackPackFrameWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;

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
					SNew(SPSE_LYFE_ItemSlotWidget)
					.InventoryPtr(InventoryPtr)
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
							.Justification(ETextJustify::Right)
							.Text(FText::FromString("PLAYER INVENTORY"))
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
								//.ScrollBarStyle(&InventoryPtr->DefaultScrollBarStyle)
								//.Style(&InventoryPtr->DefaultScrollBoxStyle)
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
							+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(FMargin(0, 0, 0, 8))
								[
									SNew(STextBlock)
									.Font(InventoryPtr->Detial1Font)
									.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
									.Justification(ETextJustify::Left)
									.Text(FText::FromString("Military Grade Backpack Stats"))
								]
							+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(FMargin(0, 0, 8, 0))
								[
									SNew(STextBlock)
									.Font(InventoryPtr->Detial2Font)
									.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
									.Justification(ETextJustify::Left)
									.Text(GetBackPackSlots())
								]
							+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(STextBlock)
									.Font(InventoryPtr->Detial2Font)
									.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
									.Justification(ETextJustify::Left)
									.Text(GetBackPackWeight())
								]
							+ SVerticalBox::Slot()
								.AutoHeight()
								.Padding(FMargin(0, 21, 0, 0))
								.HAlign(HAlign_Center)
								[
									SNew(SBorder)
									.VAlign(VAlign_Center)
									.HAlign(HAlign_Center)
									.BorderImage(&InventoryPtr->CloseButtonImg)
									.Padding(FMargin(11, 6, 11, 7))
									.OnMouseButtonDown(this, &SPSE_LYFE_BackPackFrameWidget::OnMouseCloseButtonDown)
									[
										SNew(STextBlock)
										.Font(InventoryPtr->Detial1Font)
										.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
										.Text(FText::FromString("Close"))
									]
								]
						]
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPSE_LYFE_BackPackFrameWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

FReply SPSE_LYFE_BackPackFrameWidget::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InMyGeometry, InMouseEvent);
}

FReply SPSE_LYFE_BackPackFrameWidget::OnMouseCloseButtonDown(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (InventoryPtr->InventoryState == EInventoryState::InventoryOpen)
		{
			InventoryPtr->CloseInventory();
		}
		else if (InventoryPtr->InventoryState == EInventoryState::StorageOpen)
		{
			InventoryPtr->CloseStorage();
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

const FText SPSE_LYFE_BackPackFrameWidget::GetBackPackSlots() const
{
	const uint16 MaxSlots = InventoryPtr->GetTotalBackPackSize();
	const uint16 UsedSlots = InventoryPtr->GetNoOfUsedSlots();
	return FText::FromString(FString("Available Slots in Inventory : " + FString::FromInt(UsedSlots) + "/" + FString::FromInt(MaxSlots)));
}

const FText SPSE_LYFE_BackPackFrameWidget::GetBackPackWeight() const
{
	const uint16 MaxWeight = 25;
	const uint16 CurrentWeight = InventoryPtr->TotalItemsWeight;
	return FText::FromString(FString("Backpack Weight:  : " + FString::FromInt(CurrentWeight) + "/" + FString::FromInt(MaxWeight) + " KGs"));
}