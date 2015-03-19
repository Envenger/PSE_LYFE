// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "Player/HUD/SlateWidgets/SPlayerInventoryWidget.h"
#include "PSE_LYFE_TPSHUD.generated.h"

class APSE_LYFE_ArmedCharacter;

UCLASS()
class PSE_LYFE_API APSE_LYFE_TPSHUD : public AHUD
{
	GENERATED_BODY()

public:

	APSE_LYFE_TPSHUD(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	void PostBeginPlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Font)
	UFont* UE4Font;

	void CreateInventory();

	void CloseInventory();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	/** Crosshair asset pointer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CrossHair)
	UTexture2D* CrosshairTex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MinMap)
	UMaterialInterface* TeamBlue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MinMap)
	UMaterialInterface* TeamRed;

	void DrawMinMap(UCanvas* Canvas);

	/** Store the view port where the slate widgets are drawn */
	UPROPERTY()
	UGameViewportClient* Viewport;

	UPROPERTY()
	APSE_LYFE_ArmedCharacter* OwningCharacter;
	
private:

	/** pointer storing the game slate Ui */
	TSharedPtr<class SPlayerInventoryWidget> MainInventoryUI;
};
