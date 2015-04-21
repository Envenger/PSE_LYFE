// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "Player/Inventory/Slate/Widgets/Slots/SPSE_LYFE_EquipSlotWidget.h"
#include "SPSE_LYFE_EquipmentFrameWidget.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SPSE_LYFE_EquipmentFrameWidget::Construct(const FArguments& InArgs)
{
	InventoryPtr = InArgs._InventoryPtr;
	uint8 NoOfSlots = InventoryPtr->EquipmentStorage.Num();

	bIsDisplayRotating = false;

	TSharedPtr<SVerticalBox> Container;
	SAssignNew(Container, SVerticalBox);
	for (uint8 i = 0; i < NoOfSlots; i++)
	{
		Container->AddSlot()
			.Padding(3)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SPSE_LYFE_EquipSlotWidget)
				.InventoryPtr(InventoryPtr)
				.SlotIndex(i)
			];
	}

	float SlotBottomPadding = 6;

	ChildSlot
		[
			SNew(SBox)
			.WidthOverride(InventoryPtr->InventoryWidth)
			.HeightOverride((InventoryPtr->InventoryWidth)*1.35)
			[
				SNew(SBorder)
				.VAlign(VAlign_Fill)
				.HAlign(HAlign_Fill)
				.Padding(FMargin(37.5, 33, 38.5, 3.5))
				.BorderImage(&InventoryPtr->InventoryBorderOuter)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					[
						SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.AutoHeight()
						.Padding(FMargin(0, 0, 10, 0))
						[
							SNew(STextBlock)
							.Font(InventoryPtr->InventoryHeadlineFont)
							.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
							.Justification(ETextJustify::Left)
							.Text(FString("PLAYER EQUIPMENT"))
						]
						+ SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBox)
								.HAlign(HAlign_Fill)
								.VAlign(VAlign_Center)
								[
									SNew(SImage)
									.Image(&InventoryPtr->BorderLine)
								]
							]
					]
					+ SVerticalBox::Slot()
						.Padding(FMargin(0, 13, 0, 0))
						.VAlign(VAlign_Fill)
						.HAlign(HAlign_Fill)
						.AutoHeight()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.VAlign(VAlign_Top)
							.HAlign(HAlign_Fill)
							[
								SNew(SBox)
								.VAlign(VAlign_Top)
								.HAlign(HAlign_Left)
								[
									SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.Padding(FMargin(0, 0, 0, SlotBottomPadding))
									.AutoHeight()
									[
										SNew(SVerticalBox)
										+ SVerticalBox::Slot()
										.AutoHeight()
										[
											SNew(SPSE_LYFE_EquipSlotWidget)
											.InventoryPtr(InventoryPtr)
											.SlotIndex(0)
										]
										+ SVerticalBox::Slot()
											.AutoHeight()
											[
												SNew(STextBlock)
												.Font(InventoryPtr->Detial2Font)
												.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
												.Justification(ETextJustify::Left)
												.Text(FString("Item1"))
											]
									]
									+ SVerticalBox::Slot()
										.Padding(FMargin(0, 0, 0, SlotBottomPadding))
										.AutoHeight()
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											[
												SNew(SPSE_LYFE_EquipSlotWidget)
												.InventoryPtr(InventoryPtr)
												.SlotIndex(1)
											]
											+ SVerticalBox::Slot()
												.AutoHeight()
												[
													SNew(STextBlock)
													.Font(InventoryPtr->Detial2Font)
													.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
													.Justification(ETextJustify::Left)
													.Text(FString("Item1"))
												]
										]
									+ SVerticalBox::Slot()
										.Padding(FMargin(0, 0, 0, SlotBottomPadding))
										.AutoHeight()
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											[
												SNew(SPSE_LYFE_EquipSlotWidget)
												.InventoryPtr(InventoryPtr)
												.SlotIndex(2)
											]
											+ SVerticalBox::Slot()
												.AutoHeight()
												[
													SNew(STextBlock)
													.Font(InventoryPtr->Detial2Font)
													.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
													.Justification(ETextJustify::Left)
													.Text(FString("Item1"))
												]
										]
									+ SVerticalBox::Slot()
										.Padding(FMargin(0, 0, 0, SlotBottomPadding))
										.AutoHeight()
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											[
												SNew(SPSE_LYFE_EquipSlotWidget)
												.InventoryPtr(InventoryPtr)
												.SlotIndex(3)
											]
											+ SVerticalBox::Slot()
												.AutoHeight()
												[
													SNew(STextBlock)
													.Font(InventoryPtr->Detial2Font)
													.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
													.Justification(ETextJustify::Left)
													.Text(FString("Item1"))
												]
										]
								]
							]
							+ SHorizontalBox::Slot()
								.VAlign(VAlign_Center)
								.HAlign(HAlign_Center)
								.AutoWidth()
								[
									SNew(SBorder)
									.VAlign(VAlign_Fill)
									.HAlign(HAlign_Fill)
									.BorderImage(&InventoryPtr->InventoryBorderOuter)
									.Padding(FMargin(0, 0, 0, 0))
									[
										SNew(SBox)
										.VAlign(VAlign_Fill)
										.HAlign(HAlign_Fill)
										.WidthOverride(200)
										.HeightOverride((200)*1.6)
										[
											SNew(SImage)
											.OnMouseButtonDown(this, &SPSE_LYFE_EquipmentFrameWidget::OnDisplayPressed)
											.Image(&InventoryPtr->DisplayImage)
										]
									]
								]
							+ SHorizontalBox::Slot()
								.VAlign(VAlign_Top)
								.HAlign(HAlign_Fill)
								[
									SNew(SBox)
									.VAlign(VAlign_Top)
									.HAlign(HAlign_Right)
									[
										SNew(SVerticalBox)
										+ SVerticalBox::Slot()
										.Padding(FMargin(0, 0, 0, SlotBottomPadding))
										.AutoHeight()
										[
											SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.AutoHeight()
											[
												SNew(SPSE_LYFE_EquipSlotWidget)
												.InventoryPtr(InventoryPtr)
												.SlotIndex(4)
											]
											+ SVerticalBox::Slot()
												.AutoHeight()
												[
													SNew(STextBlock)
													.Font(InventoryPtr->Detial2Font)
													.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
													.Justification(ETextJustify::Right)
													.Text(FString("Item1"))
												]
										]
										+ SVerticalBox::Slot()
											.Padding(FMargin(0, 0, 0, SlotBottomPadding))
											.AutoHeight()
											[
												SNew(SVerticalBox)
												+ SVerticalBox::Slot()
												.AutoHeight()
												[
													SNew(SPSE_LYFE_EquipSlotWidget)
													.InventoryPtr(InventoryPtr)
													.SlotIndex(5)
												]
												+ SVerticalBox::Slot()
													.AutoHeight()
													[
														SNew(STextBlock)
														.Font(InventoryPtr->Detial2Font)
														.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
														.Justification(ETextJustify::Right)
														.Text(FString("Item1"))
													]
											]
										+ SVerticalBox::Slot()
											.Padding(FMargin(0, 0, 0, SlotBottomPadding))
											.AutoHeight()
											[
												SNew(SVerticalBox)
												+ SVerticalBox::Slot()
												.AutoHeight()
												[
													SNew(SPSE_LYFE_EquipSlotWidget)
													.InventoryPtr(InventoryPtr)
													.SlotIndex(6)
												]
												+ SVerticalBox::Slot()
													.AutoHeight()
													[
														SNew(STextBlock)
														.Font(InventoryPtr->Detial2Font)
														.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
														.Justification(ETextJustify::Right)
														.Text(FString("Item1"))
													]
											]
										+ SVerticalBox::Slot()
											.Padding(FMargin(0, 0, 0, SlotBottomPadding))
											.AutoHeight()
											[
												SNew(SVerticalBox)
												+ SVerticalBox::Slot()
												.AutoHeight()
												[
													SNew(SPSE_LYFE_EquipSlotWidget)
													.InventoryPtr(InventoryPtr)
													.SlotIndex(7)
												]
												+ SVerticalBox::Slot()
													.AutoHeight()
													[
														SNew(STextBlock)
														.Font(InventoryPtr->Detial2Font)
														.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
														.Justification(ETextJustify::Right)
														.Text(FString("Item1"))
													]
											]
									]
								]
						]
					+ SVerticalBox::Slot()
						.Padding(FMargin(0, 10, 0, 0))
						.AutoHeight()
						[
							SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.Padding(FMargin(0, 0, 0, 2))
							.AutoHeight()
							[
								SNew(STextBlock)
								.Font(InventoryPtr->InventoryHeadlineFont)
								.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
								.Justification(ETextJustify::Left)
								.Text(FString("PLAYER NAME"))
							]
							+ SVerticalBox::Slot()
								.Padding(FMargin(0, 0, 0, 6))
								.AutoHeight()
								[
									SNew(STextBlock)
									.Font(InventoryPtr->Detial2Font)
									.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
									.Justification(ETextJustify::Left)
									.Text(FString("PLAYER STATS MESSAGE"))
								]
							+ SVerticalBox::Slot()
								.AutoHeight()
								[
									SNew(STextBlock)
									.Font(InventoryPtr->Detial1Font)
									.ColorAndOpacity(FSlateColor(FLinearColor(1, 1, 1, 0.75)))
									.Justification(ETextJustify::Left)
									.Text(FString("DAYS SURVIVED : 1 Minutes"))
								]
						]
				]
			]
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

FReply SPSE_LYFE_EquipmentFrameWidget::OnDisplayPressed(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (bIsDisplayRotating == false)
	{
		bIsDisplayRotating = true;
		InventoryPtr->StartDisplayActorRotate();
	}
	return FReply::Handled();
}


FReply SPSE_LYFE_EquipmentFrameWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled();
	}
	else
	{
		return FReply::Unhandled();
	}
}

FReply SPSE_LYFE_EquipmentFrameWidget::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	return OnMouseButtonDown(InMyGeometry, InMouseEvent);
}
