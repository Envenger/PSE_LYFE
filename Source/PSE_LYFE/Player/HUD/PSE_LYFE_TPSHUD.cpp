// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/PSE_LYFE_ArmedCharacter.h"
#include "Engine/Canvas.h"
#include "Weapons/BaseFiles/PSE_LYFE_ReloadableWeapon.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "PSE_LYFE_TPSHUD.h"

APSE_LYFE_TPSHUD::APSE_LYFE_TPSHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the crosshair texture
}

void APSE_LYFE_TPSHUD::BeginPlay()
{
	GetWorldTimerManager().SetTimer(this, &APSE_LYFE_TPSHUD::PostBeginPlay, 1, false);
}

void APSE_LYFE_TPSHUD::PostBeginPlay()
{
	if (GetOwningPawn())
	{
		OwningCharacter = Cast<APSE_LYFE_ArmedCharacter>(GetOwningPawn());
		OwningCharacter->CharacterHUD = this;
	}
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

			int32 CurrentClipAmmo = Weapon->CurrentAmmoInClip;
			int32 CurrentAmmo = Weapon->CurrentAmmo;

			FCanvasTextItem NewText(
				FVector2D(10, (Canvas->SizeY - 40)),
				FText::FromString(FString::FromInt(CurrentClipAmmo) + "/" + FString::FromInt(CurrentAmmo)),
				UE4Font,
				FColor::Black
				);
			Canvas->DrawItem(NewText);
			}

		int8 CurrentStamina = OwningCharacter->CurrentStaminaRep;
		int8 MaxStamina = OwningCharacter->MaxStamina;

		FCanvasTextItem NewText(
			FVector2D(10, (Canvas->SizeY - 25)),
			FText::FromString(FString::FromInt(CurrentStamina) + "/" + FString::FromInt(MaxStamina)),
			UE4Font,
			FColor::Green
			);
		Canvas->DrawItem(NewText);
	}

	// Draw very simple crosshair

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
		(Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
	/*
	FVector2D Size(50, 50);
	FVector2D Location(Canvas->SizeX / 2, Canvas->SizeY / 2);
	DrawMinMap(Canvas);
	DrawMaterialSimple(TeamBlue, (Location.X - Size.X / 2), (Location.Y - Size.Y / 2), Size.X, Size.Y);*/
}

void APSE_LYFE_TPSHUD::DrawMinMap(UCanvas* Canvas)
{
	FVector2D MapStartLocation(Canvas->SizeX * 0.1, Canvas->SizeY * 0.7);
	FVector2D MapSize(400, 400);
	FVector2D MapCenterLocation(MapStartLocation.X - MapSize.X / 2, MapStartLocation.Y - MapSize.Y / 2);
}

void APSE_LYFE_TPSHUD::CreateInventory()
{
	if (GEngine && GEngine->GameViewport)
	{
		Viewport = GEngine->GameViewport;

		SAssignNew(MainInventoryUI, SPlayerInventoryWidget)
			.InvStorage(OwningCharacter->InventoryPtr);
		Viewport->AddViewportWidgetContent(
			SNew(SWeakWidget).PossiblyNullContent(MainInventoryUI.ToSharedRef())
			);
	}
}