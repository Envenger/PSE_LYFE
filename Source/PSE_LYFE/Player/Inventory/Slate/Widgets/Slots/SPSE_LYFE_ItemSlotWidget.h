// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"

#pragma once

class PSE_LYFE_API SPSE_LYFE_ItemSlotWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPSE_LYFE_ItemSlotWidget)
		:_InventoryPtr()
		, _SlotLoc(FStorageLoc(-1, -1))
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_Inventory5_ExterStorage>, InventoryPtr)
	SLATE_ATTRIBUTE(FStorageLoc, SlotLoc)

	SLATE_END_ARGS()

public:

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FText GetNumOfStacks() const;

	const FSlateBrush* GetItemIconImg() const;

	const FSlateBrush* GetIconBorderImg() const;

	FReply OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnItemDoubleClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	bool bUpDownLeftClick;

private:

	TSharedPtr<SBorder> ItemBorder;

	TAttribute<FStorageLoc> SlotLoc;

	TAttribute<FText> Stacks;

	TWeakObjectPtr<class APSE_LYFE_Inventory5_ExterStorage> InventoryPtr;
};
