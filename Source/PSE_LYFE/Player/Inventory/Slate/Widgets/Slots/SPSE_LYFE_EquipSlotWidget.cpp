// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "SPSE_LYFE_EquipSlotWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_EquipSlotWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;
	SlotIndex = InArgs._SlotIndex;
	SlotType = InventoryPtr->EquipmentSlots[SlotIndex.Get()];

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
				.BorderImage(this, &SPSE_LYFE_EquipSlotWidget::GetItemIcon)
				.Padding(FMargin(0))
				[
					SNew(SBorder)// Has On Hover that changes the outline, has methods to change the 
					.Padding(FMargin(0, 0, 4.2, 2))
					.BorderImage(&InventoryPtr->NothingCursor)
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Bottom)
					.OnMouseButtonDown(this, &SPSE_LYFE_EquipSlotWidget::OnItemPressed)
					.OnMouseButtonUp(this, &SPSE_LYFE_EquipSlotWidget::OnItemReleased)
					.OnMouseDoubleClick(this, &SPSE_LYFE_EquipSlotWidget::OnItemDoubleClicked)
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SPSE_LYFE_EquipSlotWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpDownLeftClick == true)
	{
		if (!ItemBorder->IsHovered())
		{
			bUpDownLeftClick = false;
		}
	}
}

FReply SPSE_LYFE_EquipSlotWidget::OnItemDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return OnItemPressed(MyGeometry, MouseEvent);
}

FReply SPSE_LYFE_EquipSlotWidget::OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			InventoryPtr->EquipmentSlotLeftClick(SlotIndex.Get());
			bUpDownLeftClick = true;
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			// RightClick function
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPSE_LYFE_EquipSlotWidget::OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (bUpDownLeftClick == false)
			{
				InventoryPtr->EquipmentSlotLeftClick(SlotIndex.Get());
			}
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

const FSlateBrush* SPSE_LYFE_EquipSlotWidget::GetItemIcon() const
{
	uint8 ItemIndex = SlotIndex.Get();
	if (!InventoryPtr.Get() || InventoryPtr->EquipmentStorage[ItemIndex].ItemClass == nullptr)
	{
		return(&InventoryPtr->EmptySlotImg);
	}
	else
	{
		const APSE_LYFE_BaseInventoryItem* BaseItem = InventoryPtr->EquipmentStorage[ItemIndex].GetDefaultItem();
		return(&BaseItem->ItemIcon);
	}
}