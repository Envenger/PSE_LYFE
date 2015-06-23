// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"
#include "SPSE_LYFE_PlayerUIWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_PlayerUIWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;

	ChildSlot
	[
		SAssignNew(InventoryOverlay, SOverlay)
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPSE_LYFE_PlayerUIWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		InventoryPtr->ThrowCursorItem();
		return FReply::Handled();
	}
	else
	{
		InventoryPtr->ResetItemLastLocation();
		return FReply::Handled();
	}
}

FReply SPSE_LYFE_PlayerUIWidget::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InMyGeometry, InMouseEvent);
}

FReply SPSE_LYFE_PlayerUIWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == FKey("I") || InKeyEvent.GetKey() == FKey("Escape"))
	{ 
		if (InventoryPtr->InventoryState == EInventoryState::InventoryOpen)
		{
			InventoryPtr->CloseInventory();
		}
		else if (InventoryPtr->InventoryState == EInventoryState::StorageOpen)
		{
			InventoryPtr->CloseStorage();
		}
	}
	else if (InKeyEvent.GetKey() == FKey("LeftAlt") || InKeyEvent.GetKey() == FKey("RightAlt"))
	{
		InventoryPtr->AltPressed();
	}
	return FReply::Handled();
}

FReply SPSE_LYFE_PlayerUIWidget::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == FKey("LeftAlt") || InKeyEvent.GetKey() == FKey("RightAlt"))
	{
		InventoryPtr->AltReleased();
	}
	return FReply::Handled();
}

bool SPSE_LYFE_PlayerUIWidget::SupportsKeyboardFocus() const
{
	return true;
}