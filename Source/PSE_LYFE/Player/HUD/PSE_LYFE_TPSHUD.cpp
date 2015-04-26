// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "Engine/Canvas.h"
#include "Weapons/BaseFiles/PSE_LYFE_ReloadableWeapon.h"
#include "Player/HUD/Slate/Widget/SPSE_LYFE_PlayerUIWidget.h"
#include "Player/Inventory/Slate/Widgets/Frames/SPSE_LYFE_EquipmentFrameWidget.h"
#include "Player/Inventory/Slate/Widgets/Frames/SPSE_LYFE_StorageFrameWidget.h"
#include "Player/Inventory/Slate/Widgets/Frames/SPSE_LYFE_QuickUseFrameWidget.h"
#include "Player/Inventory/Slate/Widgets/Slots/SPSE_LYFE_CursorSlotWidget.h"
#include "TextureResource.h"
#include "CanvasItem.h"

#include "PSE_LYFE_TPSHUD.h"

APSE_LYFE_TPSHUD::APSE_LYFE_TPSHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bIsInventoryOpen = false;
	bIsMousePosLocked = false;
	// Set the crosshair texture
}

bool APSE_LYFE_TPSHUD::CreateUI()
{
	if (GEngine && GEngine->GameViewport && InventoryPtr)
	{
		Viewport = GEngine->GameViewport;

		SAssignNew(MainPlayerUI, SPSE_LYFE_PlayerUIWidget)
		.InventoryPtr(InventoryPtr);

		Viewport->AddViewportWidgetContent(
		SAssignNew(MainPlayerUIContainer, SWeakWidget)
		.PossiblyNullContent(MainPlayerUI.ToSharedRef()));

		MainPlayerUI.Get()->InventoryOverlay.Get()->AddSlot(0)// Creating the quick slots
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Right)
		.Padding(FMargin(0, 0, 3, 0))
		[
			SAssignNew(QuickSlotUI, SPSE_LYFE_QuickUseFrameWidget)
			.InventoryPtr(InventoryPtr)
		];

		MainPlayerUI.Get()->SetVisibility(EVisibility::HitTestInvisible);

		//Create quickslot here

		FInputModeGameOnly GameMode;
		GetOwningPlayerController()->SetInputMode(GameMode);

		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Creation scuccess");
		return true;
	}
	return false;
}

void APSE_LYFE_TPSHUD::DrawHUD()
{
	Super::DrawHUD();

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	if (OwningCharacter)
	{
		FVector Origin;
		FVector Direction;
		Canvas->Deproject(Center, Origin, Direction);
		OwningCharacter->ViewOrigin = Origin;
		OwningCharacter->ViewDirection = Direction;
		if (OwningCharacter->Role < ROLE_Authority)
		{
			OwningCharacter->ServerSetAim(Origin, Direction);
		}
		if (OwningCharacter->GetCurrentWeapon())
			if (OwningCharacter->GetCurrentWeapon()->IsA(APSE_LYFE_ReloadableWeapon::StaticClass()))
			{
			APSE_LYFE_ReloadableWeapon* Weapon = Cast<APSE_LYFE_ReloadableWeapon>(OwningCharacter->GetCurrentWeapon());

			int32 CurrentAmmo = Weapon->CurrentAmmo;
			int32 NoOfClips = Weapon->NoOfClips;

			FCanvasTextItem NewText(
				FVector2D(10, (Canvas->SizeY - 40)),
				FText::FromString(FString::FromInt(CurrentAmmo) + "/" + FString::FromInt(NoOfClips)),
				UE4Font,
				FColor::Black
				);
			Canvas->DrawItem(NewText);
			}

		int8 CurrentStamina = OwningCharacter->GetCurrentStaminaDisplay();
		int8 MaxStamina = OwningCharacter->MaxStamina;

		FCanvasTextItem NewText(
			FVector2D(10, (Canvas->SizeY - 25)),
			FText::FromString(FString::FromInt(CurrentStamina) + "/" + FString::FromInt(MaxStamina)),
			UE4Font,
			FColor::Green
			);
		Canvas->DrawItem(NewText);

		if (bIsMousePosLocked == true)
		{
			DetectLockMousePosition();
		}
	}

	// Draw very simple crosshair

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
		(Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

}

void APSE_LYFE_TPSHUD::CreateInventory()
{
	if (Viewport && InventoryPtr && MainPlayerUI.IsValid() && bIsInventoryOpen == false)
	{
		MainPlayerUI.Get()->SetVisibility(EVisibility::Visible);

		MainPlayerUI.Get()->InventoryOverlay.Get()->AddSlot(1)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SAssignNew(InventoryHorizontalBox, SHorizontalBox)
		];

		InventoryHorizontalBox.Get()->AddSlot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(FMargin(5))
		[
			SAssignNew(EquipmentUI, SPSE_LYFE_EquipmentFrameWidget)
			.InventoryPtr(InventoryPtr)
		];

		InventoryHorizontalBox.Get()->AddSlot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		.Padding(FMargin(5))
		[
			SAssignNew(StorageUI, SPSE_LYFE_StorageFrameWidget)
			.InventoryPtr(InventoryPtr)
		];

		SAssignNew(CursorItemUI, SPSE_LYFE_CursorSlotWidget)
			.InventoryPtr(InventoryPtr);

		Viewport->AddViewportWidgetContent(
		SAssignNew(CursorItemUIContainer, SWeakWidget)
		.PossiblyNullContent(CursorItemUI.ToSharedRef()));

		CursorItemUI.Get()->SetVisibility(EVisibility::HitTestInvisible);

		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(MainPlayerUI);
		GetOwningPlayerController()->SetInputMode(Mode);

		GetOwningPlayerController()->bShowMouseCursor = true;

		bIsInventoryOpen = true;
	}
}

void APSE_LYFE_TPSHUD::CloseInventory()
{
	if (bIsInventoryOpen == true)
	{
		if (MainPlayerUI.IsValid())
		{
			MainPlayerUI.Get()->InventoryOverlay.Get()->RemoveSlot(1);
			MainPlayerUI.Get()->SetVisibility(EVisibility::HitTestInvisible);
		}
		if (CursorItemUIContainer.IsValid())
		{
			Viewport->RemoveViewportWidgetContent(CursorItemUIContainer.ToSharedRef());
		}
		if (InventoryPtr->bIsDisplayRotateActive == true)
		{
			InventoryPtr->EndDisplayActorRotate();
		}
		FInputModeGameOnly GameMode;
		GetOwningPlayerController()->SetInputMode(GameMode);

		GetOwningPlayerController()->bShowMouseCursor = false;

		FSlateApplication::Get().SetAllUserFocusToGameViewport();
		bIsInventoryOpen = false;
	}
}

void APSE_LYFE_TPSHUD::StartDisplayActorRotate()
{
	if (GetOwningPlayerController())
	{
		APlayerController* PC = GetOwningPlayerController();
		PC->bShowMouseCursor = false;
		if (PC->GetMousePosition(MouseLockPosition.X, MouseLockPosition.Y))
		{
			bIsMousePosLocked = true;
		}
		CursorItemUI.Get()->SetVisibility(EVisibility::Visible);
	}
}

void APSE_LYFE_TPSHUD::EndDisplayActorRotate()
{
	if (GetOwningPlayerController())
	{
		APlayerController* PC = GetOwningPlayerController();
		PC->bShowMouseCursor = true;
	}
	CursorItemUI.Get()->SetVisibility(EVisibility::HitTestInvisible);
	EquipmentUI->bIsDisplayRotating = false;
	bIsMousePosLocked = false;	
}

void APSE_LYFE_TPSHUD::DetectLockMousePosition()
{
	if (GetOwningPlayerController())
	{
		APlayerController* PC = GetOwningPlayerController();
		FVector2D NewMousePosition;
		if (PC->GetMousePosition(NewMousePosition.X, NewMousePosition.Y))
		{
			if (InventoryPtr)
			{
				InventoryPtr->RotateDisplayActor(MouseLockPosition.X - NewMousePosition.X);
			}
			FViewport* MainViewPort = Viewport->Viewport;
			MainViewPort->SetMouse(MouseLockPosition.X, MouseLockPosition.Y);
		}
	}
}