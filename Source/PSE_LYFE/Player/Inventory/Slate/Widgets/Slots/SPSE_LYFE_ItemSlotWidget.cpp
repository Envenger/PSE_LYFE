// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "SPSE_LYFE_ItemSlotWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_ItemSlotWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;
	SlotLoc = InArgs._SlotLoc;

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
				.BorderImage(this, &SPSE_LYFE_ItemSlotWidget::GetItemIconImg)
				.Padding(FMargin(1))
				[
					SNew(SBorder)
					.BorderImage(this, &SPSE_LYFE_ItemSlotWidget::GetIconBorderImg)
					[
						SNew(SBorder)// Has On Hover that changes the outline, has methods to change the 
						.Padding(FMargin(0, 0, 4.2, 2))
						.BorderImage(&InventoryPtr->NothingCursor)
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Bottom)
						.OnMouseButtonDown(this, &SPSE_LYFE_ItemSlotWidget::OnItemPressed)
						.OnMouseButtonUp(this, &SPSE_LYFE_ItemSlotWidget::OnItemReleased)
						.OnMouseDoubleClick(this, &SPSE_LYFE_ItemSlotWidget::OnItemDoubleClicked)
						[
							SNew(STextBlock)
							.Visibility(EVisibility::Visible)
							.Font(InventoryPtr->StacksFont)
							.ColorAndOpacity(FSlateColor(FLinearColor(2.5, 0.5, 0.5)))
							.Text(this, &SPSE_LYFE_ItemSlotWidget::GetNumOfStacks)
						]
					]
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPSE_LYFE_ItemSlotWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpDownLeftClick == true)
	{
		if (!ItemBorder->IsHovered())
		{
			bUpDownLeftClick = false;
		}
	}
}

FReply SPSE_LYFE_ItemSlotWidget::OnItemDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return OnItemPressed(MyGeometry, MouseEvent);
}

FReply SPSE_LYFE_ItemSlotWidget::OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!InventoryPtr->BackPack.IsValidIndex(ItemLoc))
	{
		return FReply::Handled();
	}
	if (InventoryPtr->BackPack.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, "Slot disabled");
		return FReply::Handled();
	}
	else if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			InventoryPtr->BackPackSlotLeftClick(ItemLoc);
			bUpDownLeftClick = true;
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			InventoryPtr->BackPackSlotRightClick(ItemLoc);
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPSE_LYFE_ItemSlotWidget::OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!InventoryPtr->BackPack.IsValidIndex(ItemLoc))
	{
		return FReply::Handled();
	}
	if (InventoryPtr->BackPack.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, "Slot disabled");
		return FReply::Handled();
	}
	else if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (bUpDownLeftClick == false)
			{
				InventoryPtr->BackPackSlotLeftClick(ItemLoc);
			}
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FString SPSE_LYFE_ItemSlotWidget::GetNumOfStacks() const
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!InventoryPtr->BackPack.IsValidIndex(ItemLoc))
	{
		return("");
	}
	if (InventoryPtr->BackPack.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
	{
		return("");
	}
	if (InventoryPtr->BackPack.GetItem(SlotLoc.Get()).ItemClass == nullptr)
	{
		return("");
	}
	const APSE_LYFE_BaseInventoryItem* BaseItem = InventoryPtr->BackPack.GetItem(ItemLoc).GetDefaultItem();
	if (BaseItem->GetItemType() == EItemType::StackableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
	{
		const int32 CurrentStacks = InventoryPtr->BackPack.GetItem(ItemLoc).GetStacks();
		if (CurrentStacks > 1)
		{
			return(FString::FromInt(CurrentStacks));
		}
		return("");
	}
	return("");
}

const FSlateBrush* SPSE_LYFE_ItemSlotWidget::GetItemIconImg() const
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!InventoryPtr->BackPack.IsValidIndex(ItemLoc))
	{
		return(&InventoryPtr->DisabledSlotImg);
	}
	if (InventoryPtr->BackPack.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
	{
		return(&InventoryPtr->DisabledSlotImg);
	}
	if (!InventoryPtr->BackPack.StorageBasePtr || InventoryPtr->BackPack.GetItem(ItemLoc).ItemClass == nullptr)
	{
		return(&InventoryPtr->EmptySlotImg);
	}
	else
	{
		const APSE_LYFE_BaseInventoryItem* BaseItem = InventoryPtr->BackPack.GetItem(ItemLoc).GetDefaultItem();
		return(BaseItem->GetItemIcon());
	}
}

const FSlateBrush* SPSE_LYFE_ItemSlotWidget::GetIconBorderImg() const
{
	if (ItemBorder->IsHovered())
	{
		return(&InventoryPtr->ItemIconSelectedBorder);
	}
	else
	{
		return(&InventoryPtr->ItemIconDefaultBorder);
	}
}