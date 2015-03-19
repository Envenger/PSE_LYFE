// Fill out your copyright notice in the Description page of Project Settings.
#include "Player/Storage/PSE_LYFE_PlayerInventory.h"

#pragma once

/**
 * 
 */
class PSE_LYFE_API SItemWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SItemWidget)
		:_InvStorage()
		, _SlotLoc(FStorageLoc(-1, -1))
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class APSE_LYFE_PlayerInventory>, InvStorage)
		SLATE_ATTRIBUTE(FStorageLoc, SlotLoc)

		SLATE_END_ARGS()

public:

	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	FString GetNumOfStacks() const;

	const FSlateBrush* GetItemIcon() const;

	FReply OnItemPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	FReply OnItemReleased(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	bool bUpDownLeftClick;

private:

	TSharedPtr<SBorder> ItemBorder;

	TAttribute<FStorageLoc> SlotLoc;

	TAttribute<FText> Stacks;

	TWeakObjectPtr<class APSE_LYFE_PlayerInventory> InvStorage;
};
