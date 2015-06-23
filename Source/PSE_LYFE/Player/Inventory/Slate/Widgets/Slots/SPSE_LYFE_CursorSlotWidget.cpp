// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "SPSE_LYFE_CursorSlotWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_CursorSlotWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = TWeakObjectPtr<APSE_LYFE_Inventory4_QuickSlots>(InArgs._InventoryPtr);

	IconLocation.Bind(this, &SPSE_LYFE_CursorSlotWidget::GetCursorLocation);

	ChildSlot
		[
			SNew(SCanvas)
			+ SCanvas::Slot()
			.Position(IconLocation)
			.Size(FVector2D(InventoryPtr->InventoryIconSize, InventoryPtr->InventoryIconSize))
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(this, &SPSE_LYFE_CursorSlotWidget::GetItemIcon)
				.Padding(FMargin(0, 0, 4.2, 2))
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Bottom)
				[
					SNew(STextBlock)
					.Font(InventoryPtr->StacksFont)
					.ColorAndOpacity(FSlateColor(FLinearColor(2.5, 0.5, 0.5)))
					.Text(this, &SPSE_LYFE_CursorSlotWidget::GetNumOfStacks)
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FString SPSE_LYFE_CursorSlotWidget::GetNumOfStacks() const
{

	if (InventoryPtr->CursorItem.ItemClass == nullptr)
	{
		return("");
	}
	const APSE_LYFE_BaseInventoryItem* BaseItem = InventoryPtr->CursorItem.GetDefaultItem();
	if (BaseItem->GetItemType() == EItemType::StackableItem || BaseItem->GetItemType() == EItemType::StackableUsableItem)
	{
		const int32 CurrentStacks = InventoryPtr->CursorItem.ItemProperties[0];
		if (CurrentStacks > 1)
		{
			return(FString::FromInt(CurrentStacks));
		}
		return("");
	}
	return("");
}

const FSlateBrush* SPSE_LYFE_CursorSlotWidget::GetItemIcon() const
{
	if (InventoryPtr->CursorItem.ItemClass == nullptr)
	{
		return(&InventoryPtr->NothingCursor);
	}
	else
	{
		const APSE_LYFE_BaseInventoryItem* BaseItem = InventoryPtr->CursorItem.GetDefaultItem();
		return BaseItem->GetItemIcon();
	}
}

void SPSE_LYFE_CursorSlotWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	//Super::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
	CachedGeometry = AllottedGeometry;
}

FVector2D SPSE_LYFE_CursorSlotWidget::GetCursorLocation() const
{
	FVector2D MouseLoc = FSlateApplication::Get().GetCursorPos();

	MouseLoc = CachedGeometry.AbsoluteToLocal(MouseLoc);
	MouseLoc -= FVector2D(32, 32);
	return(MouseLoc);
	//return(InvStorage->CursorLocation);
}

FReply SPSE_LYFE_CursorSlotWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if(InventoryPtr->bIsDisplayRotateActive == true)
	{
		InventoryPtr->EndDisplayActorRotate();
	}
	return FReply::Handled();
}