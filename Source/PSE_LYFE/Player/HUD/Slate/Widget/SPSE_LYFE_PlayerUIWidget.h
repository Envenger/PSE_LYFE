// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SPSE_LYFE_PlayerUIWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPSE_LYFE_PlayerUIWidget)
		:_InventoryPtr()
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots>, InventoryPtr)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/** The box contaning the inventory */
	TSharedPtr<SOverlay> InventoryOverlay;

	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	virtual bool SupportsKeyboardFocus() const override;

private:

	FReply OnHUDPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnHUDReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnHUDDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots> InventoryPtr;
};
