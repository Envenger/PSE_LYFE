// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SPSE_LYFE_QuickUseSlotWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPSE_LYFE_QuickUseSlotWidget)
		:_InventoryPtr()
		, _QuickSlotIndex(0)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots>, InventoryPtr)
	SLATE_ATTRIBUTE(uint8, QuickSlotIndex)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	FString GetNumOfStacks() const;

	const FSlateBrush* GetItemIcon() const;

	FReply OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnItemDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	bool bUpDownLeftClick;

private:

	TSharedPtr<SBorder> ItemBorder;

	TAttribute<uint8> QuickSlotIndex;

	TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots> InventoryPtr;
};
