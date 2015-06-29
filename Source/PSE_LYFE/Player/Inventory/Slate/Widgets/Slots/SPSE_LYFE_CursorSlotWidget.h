// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SPSE_LYFE_CursorSlotWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPSE_LYFE_CursorSlotWidget)
		:_InventoryPtr()
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots>, InventoryPtr)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	TAttribute<FVector2D> IconLocation;

	FVector2D GetCursorLocation() const;

	FText GetNumOfStacks() const;

	const FSlateBrush* GetItemIcon() const;

	FGeometry CachedGeometry;

	TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots> InventoryPtr;

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
};
