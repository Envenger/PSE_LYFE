// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SPSE_LYFE_StorgeFrameWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPSE_LYFE_StorgeFrameWidget)
		: _InventoryPtr()
		, _ExternalStoragePtr()
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_Inventory5_ExterStorage>, InventoryPtr)
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_BaseStorage>, ExternalStoragePtr)
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent) override;

	TWeakObjectPtr<class APSE_LYFE_Inventory5_ExterStorage> InventoryPtr;

	TWeakObjectPtr<class APSE_LYFE_BaseStorage> ExternalStoragePtr;
};
