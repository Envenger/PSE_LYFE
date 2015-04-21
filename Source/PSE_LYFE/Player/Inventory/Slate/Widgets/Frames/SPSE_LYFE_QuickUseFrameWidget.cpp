// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "Player/Inventory/Slate/Widgets/Slots/SPSE_LYFE_QuickUseSlotWidget.h"
#include "SPSE_LYFE_QuickUseFrameWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_QuickUseFrameWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;
	
	int8 NoOfQuickSlots = InventoryPtr->NoOfQuickSlots;

	TSharedPtr<SVerticalBox> Container;
	SAssignNew(Container, SVerticalBox);
	for (int32 i = 0; i < NoOfQuickSlots; i++)
	{
		Container->AddSlot()
			.Padding(3)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SPSE_LYFE_QuickUseSlotWidget)
				.InventoryPtr(InventoryPtr)
				.QuickSlotIndex(i)
			];
	}

	ChildSlot
		[
			SNew(SBorder)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			.Padding(FMargin(9, 10, 10, 10))
			.BorderImage(&InventoryPtr->InventoryBorderOuter)
			[
				Container.ToSharedRef()
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
