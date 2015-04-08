// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SPSE_LYFE_EquipmentFrameWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPSE_LYFE_EquipmentFrameWidget)
		:_InventoryPtr()
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots>, InventoryPtr)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	bool bIsDisplayRotating;

	FReply OnDisplayPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

private:

	TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots> InventoryPtr;
};
