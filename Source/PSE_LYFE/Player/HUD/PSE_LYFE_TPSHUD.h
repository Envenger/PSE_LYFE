// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"
#include "PSE_LYFE_TPSHUD.generated.h"

class APSE_LYFE_Character4_Weapon;

UCLASS()
class PSE_LYFE_API APSE_LYFE_TPSHUD : public AHUD
{
	GENERATED_BODY()

public:

	APSE_LYFE_TPSHUD(const FObjectInitializer& ObjectInitializer);

	bool CreateUI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Font)
	UFont* UE4Font;

	bool CreateInventoryUI();

	void CloseInventoryUI();

	bool CreateStorageUI();

	void CloseStorageUI();

	/** Used when swapping bags */
	void CreateTempBackPackSlot();

	/** Used when swapping bags */
	void CloseTempBackPackSlot();

	bool ItemThrowHoverTest();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	/** Crosshair asset pointer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CrossHair)
	UTexture2D* CrosshairTex;

	/** Store the view port where the slate widgets are drawn */
	UPROPERTY()
	UGameViewportClient* Viewport;

	UPROPERTY()
	APSE_LYFE_Character4_Weapon* OwningCharacter;

	UPROPERTY()
	APSE_LYFE_Inventory5_ExterStorage* InventoryPtr;
	//TWeakObjectPtr<class APSE_LYFE_Inventory4_QuickSlots> InventoryPtr;

	void StartDisplayActorRotate();

	void EndDisplayActorRotate();

	bool bIsMousePosLocked;

	FVector2D MouseLockPosition;

	void DetectLockMousePosition();

private:
	
	/** pointer storing the game slate UI */
	TSharedPtr<class SPSE_LYFE_PlayerUIWidget> MainPlayerUI;

	/** A Horizontal box containing the center menu widgets */
	TSharedPtr<SHorizontalBox> InventoryHorizontalBox;

	/** Scoreboard widget container - used for removing */
	TSharedPtr<class SWidget> MainPlayerUIContainer;

	/** pointer storing the game slate Ui */
	TSharedPtr<class SPSE_LYFE_BackPackFrameWidget> BackPackUI;

	/** pointer storing the game slate Ui */
	TSharedPtr<class SPSE_LYFE_EquipmentFrameWidget> EquipmentUI;

	/** pointer storing the game slate Ui */
	TSharedPtr<class SPSE_LYFE_QuickUseFrameWidget> QuickSlotUI;

	/** pointer storing the game slate Ui */
	TSharedPtr<class SPSE_LYFE_StorgeFrameWidget> StorageUI;

	/** pointer storing the game slate Ui */
	TSharedPtr<class SPSE_LYFE_CursorSlotWidget> CursorItemUI;

	/** Scoreboard widget container - used for removing */
	TSharedPtr<class SWidget> CursorItemUIContainer;



};
