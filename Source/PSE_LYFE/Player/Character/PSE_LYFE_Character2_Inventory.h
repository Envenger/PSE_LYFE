// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Character/PSE_LYFE_Character1_Movement.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "Player/Inventory/PSE_LYFE_Inventory5_ExterStorage.h"
#include "PSE_LYFE_Character2_Inventory.generated.h"


UCLASS()
class PSE_LYFE_API APSE_LYFE_Character2_Inventory : public APSE_LYFE_Character1_Movement
{
	GENERATED_BODY()

public:

	void SetupPlayerInputComponent(class UInputComponent* InputComponent);

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<APSE_LYFE_Inventory4_QuickSlots> InventoryClass;

	UPROPERTY(ReplicatedUsing = OnRep_InventoryInitialize, Transient)
	class APSE_LYFE_Inventory5_ExterStorage* InventoryPtr;

	UFUNCTION()
	void OnRep_InventoryInitialize();

	void HUDStorageOwnerLink();

	void UseItem();

	UFUNCTION(reliable, server, WithValidation)
	void Server_PickInventoryItem();
	bool Server_PickInventoryItem_Validate();
	void Server_PickInventoryItem_Implementation();

	UFUNCTION(reliable, server, WithValidation)
	void Server_OpenStorage();
	bool Server_OpenStorage_Validate();
	void Server_OpenStorage_Implementation();

	void UseInventory();

	UPROPERTY()
	APSE_LYFE_TPSHUD* CharacterHUD;

	float AllItemThrowTimer;

	const FVector GetCharacterThrowLocation() const;

	FTotalItemStruct TotalItemsArray;



};
