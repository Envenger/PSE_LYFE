// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Storage/PSE_LYFE_BaseStorage.h"
#include "SPSE_LYFE_StorageItemSlotWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_StorageItemSlotWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;
	ExternalStoragePtr = InArgs._ExternalStoragePtr;
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
				.BorderImage(this, &SPSE_LYFE_StorageItemSlotWidget::GetItemIconImg)
				.Padding(FMargin(1))
				[
					SNew(SBorder)
					.BorderImage(this, &SPSE_LYFE_StorageItemSlotWidget::GetIconBorderImg)
					[
						SNew(SBorder)// Has On Hover that changes the outline, has methods to change the 
						.Padding(FMargin(0, 0, 4.2, 2))
						.BorderImage(&InventoryPtr->NothingCursor)
						.HAlign(HAlign_Right)
						.VAlign(VAlign_Bottom)
						.OnMouseButtonDown(this, &SPSE_LYFE_StorageItemSlotWidget::OnItemPressed)
						.OnMouseButtonUp(this, &SPSE_LYFE_StorageItemSlotWidget::OnItemReleased)
						.OnMouseDoubleClick(this, &SPSE_LYFE_StorageItemSlotWidget::OnItemDoubleClicked)
						[
							SNew(STextBlock)
							.Visibility(EVisibility::Visible)
							.Font(InventoryPtr->StacksFont)
							.ColorAndOpacity(FSlateColor(FLinearColor(2.5, 0.5, 0.5)))
							.Text(this, &SPSE_LYFE_StorageItemSlotWidget::GetNumOfStacks)
						]
					]
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPSE_LYFE_StorageItemSlotWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpDownLeftClick == true)
	{
		if (!ItemBorder->IsHovered())
		{
			bUpDownLeftClick = false;
		}
	}
}

FReply SPSE_LYFE_StorageItemSlotWidget::OnItemDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return OnItemPressed(MyGeometry, MouseEvent);
}

FReply SPSE_LYFE_StorageItemSlotWidget::OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!ExternalStoragePtr->Storage.IsValidIndex(ItemLoc))
	{
		return FReply::Handled();
	}
	if (ExternalStoragePtr->Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Red, "Slot disabled");
		return FReply::Handled();
	}
	else if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			InventoryPtr->StorageSlotLeftClick(ItemLoc);
			bUpDownLeftClick = true;
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			InventoryPtr->StorageSlotRightClick(ItemLoc);
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPSE_LYFE_StorageItemSlotWidget::OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!ExternalStoragePtr->Storage.IsValidIndex(ItemLoc))
	{
		return FReply::Handled();
	}
	if (ExternalStoragePtr->Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
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
				InventoryPtr->StorageSlotLeftClick(ItemLoc);
			}
		}
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FString SPSE_LYFE_StorageItemSlotWidget::GetNumOfStacks() const
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!ExternalStoragePtr->Storage.IsValidIndex(ItemLoc))
	{
		return("");
	}
	else if (ExternalStoragePtr->Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
	{
		return("");
	}
	else if (ExternalStoragePtr->Storage.GetItem(SlotLoc.Get()).ItemClass == nullptr)
	{
		return("");
	}
	const APSE_LYFE_BaseInventoryItem* BaseItem = ExternalStoragePtr->Storage.GetItem(ItemLoc).GetDefaultItem();
	if (BaseItem->GetItemType() == EItemType::StackableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
	{
		const int32 CurrentStacks = ExternalStoragePtr->Storage.GetItem(ItemLoc).GetStacks();
		if (CurrentStacks > 1)
		{
			return(FString::FromInt(CurrentStacks));
		}
		return("");
	}
	return("");
}

const FSlateBrush* SPSE_LYFE_StorageItemSlotWidget::GetItemIconImg() const
{
	FStorageLoc ItemLoc = SlotLoc.Get();
	if (!ExternalStoragePtr->Storage.IsValidIndex(ItemLoc))
	{
		return(&InventoryPtr->DisabledSlotImg);
	}
	else if (ExternalStoragePtr->Storage.Rows[ItemLoc.RowNum].Columns[ItemLoc.ColNum].Index < 0)
	{
		return(&InventoryPtr->DisabledSlotImg);
	}
	else if (!ExternalStoragePtr->Storage.StorageBasePtr || ExternalStoragePtr->Storage.GetItem(ItemLoc).ItemClass == nullptr)
	{
		return(&InventoryPtr->EmptySlotImg);
	}
	else
	{
		const APSE_LYFE_BaseInventoryItem* BaseItem = ExternalStoragePtr->Storage.GetItem(ItemLoc).GetDefaultItem();
		return(BaseItem->GetItemIcon());
	}
}

const FSlateBrush* SPSE_LYFE_StorageItemSlotWidget::GetIconBorderImg() const
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