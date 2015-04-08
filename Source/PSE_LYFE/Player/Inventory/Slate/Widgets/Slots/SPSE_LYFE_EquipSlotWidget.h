// Fill out your copyright notice in the Description page of Project Settings.
#include "Items/PSE_LYFE_BaseInventoryItem.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SPSE_LYFE_EquipSlotWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPSE_LYFE_EquipSlotWidget)
		:_InventoryPtr()
		, _SlotIndex(-1)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots>, InventoryPtr)
	SLATE_ATTRIBUTE(int32, SlotIndex)

	SLATE_END_ARGS()

public:

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

	TAttribute<int32> SlotIndex;

	EEquipmentSlotType SlotType;

	TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots> InventoryPtr;
};
