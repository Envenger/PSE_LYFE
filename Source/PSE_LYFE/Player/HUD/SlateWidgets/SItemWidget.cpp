// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "SlateBasics.h"
#include "STextBlock.h"
#include "SBox.h"
#include "SButton.h"
#include "SItemWidget.h"
#include "Player/Storage/PSE_LYFE_PlayerInventory.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SItemWidget::Construct(const FArguments& InArgs)
{
	InvStorage = InArgs._InvStorage;
	SlotLoc = InArgs._SlotLoc;
	bUpDownLeftClick = false;

	ChildSlot
		[
			SNew(SBox)
			.HeightOverride(64)
			.WidthOverride(64)
			.Padding(FMargin(0))
			[
				SAssignNew(ItemBorder, SBorder)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.BorderImage(this, &SItemWidget::GetItemIcon)
				.Padding(FMargin(0))
				[
					SNew(SBorder)// Has On Hover that changes the outline, has methods to change the 
					.Padding(FMargin(0, 0, 4.2, 2))
					.BorderImage(&InvStorage->NothingCursor)
					.HAlign(HAlign_Right)
					.VAlign(VAlign_Bottom)
					.OnMouseButtonDown(this, &SItemWidget::OnItemPressed)
					.OnMouseButtonUp(this, &SItemWidget::OnItemReleased)
					[
						SNew(STextBlock)
						.Font(InvStorage->StacksFont)
						.ColorAndOpacity(FSlateColor(FLinearColor(2.5, 0.5, 0.5)))
						.Text(this, &SItemWidget::GetNumOfStacks)
					]
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SItemWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	if (bUpDownLeftClick == true)
	{
		if (!ItemBorder->IsHovered())
		{
			bUpDownLeftClick = false;
		}
	}
}

FReply SItemWidget::OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "LeftClick");
			InvStorage->LeftClick(SlotLoc.Get());
			bUpDownLeftClick = true;
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			InvStorage->UseItem(SlotLoc.Get());
		}
	}
	return FReply::Handled();
}

FReply SItemWidget::OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (IsEnabled())
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			if (bUpDownLeftClick == false)
			{
				InvStorage->LeftClick(SlotLoc.Get());
			}
		}
	}
	return FReply::Handled();
}

FString SItemWidget::GetNumOfStacks() const
{
	if (InvStorage->Storage.Rows[SlotLoc.Get().RowNum].Columns[SlotLoc.Get().ColNum].ItemType == EItemType::StackableSingleConsumable)
	{
		int32 ItemIndex = InvStorage->Storage.Rows[SlotLoc.Get().RowNum].Columns[SlotLoc.Get().ColNum].ItemIndex;
		int32 Stacks = InvStorage->StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem.CurrentStacks;
		if (Stacks > 1)
		{
			return(FString::FromInt(Stacks));
		}
	}
	return("");
}

const FSlateBrush* SItemWidget::GetItemIcon() const
{
	if (InvStorage->Storage.Rows[SlotLoc.Get().RowNum].Columns[SlotLoc.Get().ColNum].ItemType == EItemType::StackableSingleConsumable)
	{
		int32 ItemIndex = InvStorage->Storage.Rows[SlotLoc.Get().RowNum].Columns[SlotLoc.Get().ColNum].ItemIndex;
		return(&InvStorage->StackableSingleFoodItemArray[ItemIndex].StackableSingleFoodItem.ItemIcon);
	}
	else
	{
		return(&InvStorage->EmptySlotImg);
	}
}
