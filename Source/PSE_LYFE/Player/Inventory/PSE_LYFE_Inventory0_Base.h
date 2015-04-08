// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_Inventory0_Base.generated.h"

class APSE_LYFE_Character;

class APSE_LYFE_TPSHUD;

UCLASS()
class PSE_LYFE_API APSE_LYFE_Inventory0_Base : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	APSE_LYFE_Inventory0_Base();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetOwningPawn(APSE_LYFE_Character* NewOwner);

	UPROPERTY(replicated)
	APSE_LYFE_Character* OwningPawn;

	APSE_LYFE_TPSHUD* CharacterHUD;

///////////////////////////////////////////////

	/** Design of the board */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush InventoryBorderOuter;

	/** Design of the board */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush InventoryBorderInner;

	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush NothingCursor;

	/** Icon of an emptry slot */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush EmptySlotImg;

	/** Icon of an emptry slot */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush BorderLine;

	/** Icon of an emptry slot */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush DisabledSlotImg;

	/** Icon of an emptry slot */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush CloseButtonImg;

	/** Icon of an emptry slot */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush StorageBorderImg;

	/** Border of a nonselected Storage item */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush ItemIconDefaultBorder;

	/** Border of a selected storage item */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush ItemIconSelectedBorder;

	/** Border of a selected storage item */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateBrush DisplayImage;

	UPROPERTY(EditAnywhere, Category = Inventory)
	FScrollBoxStyle DefaultScrollBoxStyle;

	UPROPERTY(EditAnywhere, Category = Inventory)
	FScrollBarStyle DefaultScrollBarStyle;

	/** Font Background Image */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateFontInfo StacksFont;

	/** Font Background Image */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateFontInfo InventoryHeadlineFont;

	/** Font Background Image */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateFontInfo Detial1Font;

	/** Font Background Image */
	UPROPERTY(EditAnywhere, Category = Inventory)
	FSlateFontInfo Detial2Font;

	/** Width of the background invenotry height is derived from it with a ratio */
	UPROPERTY(EditAnywhere, Category = InventoryVariables)
	float InventoryWidth;

	/** Item icon size */
	UPROPERTY(EditAnywhere, Category = InventoryVariables)
	float InventoryIconSize;
	
};
