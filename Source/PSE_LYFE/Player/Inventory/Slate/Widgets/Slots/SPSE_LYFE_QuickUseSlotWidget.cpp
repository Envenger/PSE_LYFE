// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "SPSE_LYFE_QuickUseSlotWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_QuickUseSlotWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;
	QuickSlotIndex = InArgs._QuickSlotIndex;

	bUpDownLeftClick = false;

	ChildSlot
		[
			SNew(SBox)
			.HeightOverride(InventoryPtr->InventoryIconSize)
			.WidthOverride(InventoryPtr->InventoryIconSize)
			.Padding(FMargin(0))
			[
				SAssignNew(ItemBorder, SBorder)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.BorderImage(this, &SPSE_LYFE_QuickUseSlotWidget::GetItemIcon)
				.Padding(FMargin(0))
				[
					SNew(SBorder)// Has On Hover that changes the outline, has methods to change the 
					.Padding(FMargin(0, 0, 4.2, 2))
					.BorderImage(&InventoryPtr->NothingCursor)
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Bottom)
					.OnMouseButtonDown(this, &SPSE_LYFE_QuickUseSlotWidget::OnItemPressed)
					.OnMouseButtonUp(this, &SPSE_LYFE_QuickUseSlotWidget::OnItemReleased)
					.OnMouseDoubleClick(this, &SPSE_LYFE_QuickUseSlotWidget::OnItemDoubleClicked)
					[
						SNew(STextBlock)
						.Visibility(EVisibility::Visible)
						.Font(InventoryPtr->StacksFont)
						.ColorAndOpacity(FSlateColor(FLinearColor(2.5, 0.5, 0.5)))
						.Text(this, &SPSE_LYFE_QuickUseSlotWidget::GetNumOfStacks)
					]
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPSE_LYFE_QuickUseSlotWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpDownLeftClick == true)
	{
		if (!ItemBorder->IsHovered())
		{
			bUpDownLeftClick = false;
		}
	}
}

FReply SPSE_LYFE_QuickUseSlotWidget::OnItemDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return OnItemPressed(MyGeometry, MouseEvent);
}

FReply SPSE_LYFE_QuickUseSlotWidget::OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			InventoryPtr->QuickSlotLeftClick(QuickSlotIndex.Get());
			bUpDownLeftClick = true;
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			InventoryPtr->QuickSlotRightClick(QuickSlotIndex.Get());
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPSE_LYFE_QuickUseSlotWidget::OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (bUpDownLeftClick == false)
			{
				InventoryPtr->QuickSlotLeftClick(QuickSlotIndex.Get());
			}
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FString SPSE_LYFE_QuickUseSlotWidget::GetNumOfStacks() const
{
	if (InventoryPtr->QuickSlots[QuickSlotIndex.Get()] == nullptr)
	{
		return("");
	}
	const APSE_LYFE_BaseInventoryItem* BaseItem = InventoryPtr->QuickSlots[QuickSlotIndex.Get()]->GetDefaultObject<APSE_LYFE_BaseInventoryItem>();
	const int32 CurrentStacks = InventoryPtr->FindTotalItemsOfType(InventoryPtr->QuickSlots[QuickSlotIndex.Get()]);
	if (CurrentStacks > 1)
	{
		return(FString::FromInt(CurrentStacks));
	}
	return("");
}

const FSlateBrush* SPSE_LYFE_QuickUseSlotWidget::GetItemIcon() const
{
	if (InventoryPtr->QuickSlots[QuickSlotIndex.Get()] == nullptr)
	{
		return(&InventoryPtr->EmptySlotImg);
	}
	else
	{
		const APSE_LYFE_BaseInventoryItem* BaseItem = InventoryPtr->QuickSlots[QuickSlotIndex.Get()]->GetDefaultObject<APSE_LYFE_BaseInventoryItem>();
		return(BaseItem->GetItemIcon());
	}
}