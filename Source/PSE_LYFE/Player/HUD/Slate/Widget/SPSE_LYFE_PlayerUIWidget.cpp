// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "SPSE_LYFE_PlayerUIWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_PlayerUIWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;

	ChildSlot
	[
		SNew(SBorder)
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.OnMouseButtonDown(this, &SPSE_LYFE_PlayerUIWidget::OnHUDPressed)
		.OnMouseButtonUp(this, &SPSE_LYFE_PlayerUIWidget::OnHUDReleased)
		.OnMouseDoubleClick(this, &SPSE_LYFE_PlayerUIWidget::OnHUDDoubleClicked)
		[		
			SAssignNew(InventoryOverlay, SOverlay)
		]
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPSE_LYFE_PlayerUIWidget::OnHUDDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return OnHUDPressed(MyGeometry, MouseEvent);
}

FReply SPSE_LYFE_PlayerUIWidget::OnHUDPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

FReply SPSE_LYFE_PlayerUIWidget::OnHUDReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

FReply SPSE_LYFE_PlayerUIWidget::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == FKey("I") || InKeyEvent.GetKey() == FKey("Escape"))
	{
		InventoryPtr->CharacterHUD->CloseInventory();
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