// Fill out your copyright notice in the Description page of Project Settings.
#include "SlateBasics.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SPlayerInventoryWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPlayerInventoryWidget)
		:_InvStorage()
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_PlayerInventory>, InvStorage)

	SLATE_END_ARGS()

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	/** The box contaning the inventory */
	TSharedPtr<SBorder> InventoryBox;

private:
	TWeakObjectPtr<class APSE_LYFE_PlayerInventory> InvStorage;
};
