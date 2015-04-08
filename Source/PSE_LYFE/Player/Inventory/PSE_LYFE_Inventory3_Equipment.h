// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Inventory/PSE_LYFE_Inventory2_Storage.h"
#include "PSE_LYFE_Inventory3_Equipment.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_Inventory3_Equipment : public APSE_LYFE_Inventory2_Storage
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_Inventory3_Equipment();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UPROPERTY(Replicated, Repretry)
	TArray<FItemStruct> EquipmentStorage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = EquipmentSlots)
	TArray<EEquipmentSlotType> EquipmentSlots;

	/** Takes the equipment storage and initazlizes the storage base */
	void InitializeStorage();

	bool CheckSlotType(const uint8 EquipmentSlotLoc, const EEquipmentSlotType EquipmentSlotType) const;

	/** Item slot type needs to be checked before adding item or it wont work. Replaces the  */
	void AddInventoryEquipment(const uint8 EquipmentSlotLoc, FItemStruct &NewItem);

	void RemoveInventoryEquipment(const uint8 EquipmentSlotLoc);

	void EquipItem(const FStorageLoc ItemLoc) override;

	void EquipmentSlotLeftClick(const uint8 SlotLoc);

	UFUNCTION(reliable, server, WithValidation)
	void Server_EquipmentSlotLeftClick(const uint8 SlotLoc);
	bool Server_EquipmentSlotLeftClick_Validate(const uint8 SlotLoc);
	void Server_EquipmentSlotLeftClick_Implementation(const uint8 SlotLoc);

	void EquipmentSlotRightClick(const uint8 SlotLoc);

	UFUNCTION(reliable, server, WithValidation)
	void Server_EquipmentSlotRightClick(const uint8 SlotLoc);
	bool Server_EquipmentSlotRightClick_Validate(const uint8 SlotLoc);
	void Server_EquipmentSlotRightClick_Implementation(const uint8 SlotLoc);

//////////////////////////////////////////////
// Display Screen //


	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<AActor> DislayActorClass;

	FVector DisplayActorSpawnLocation;

	FRotator DisplayActorSpawnRotation;

	/** The actor which is rendered in the display screen */
	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = Inventory)
	AActor* DisplayActor;

	void StartDisplayActorRotate();

	void EndDisplayActorRotate();

	bool bIsDisplayRotateActive;

	UPROPERTY(EditDefaultsOnly, Category = InventoryVariables)
	float RotationCoefficent;

	void RotateDisplayActor(float ScreenChange);

private:

	void ItemAdded(const uint8 EquipmentSlotLoc);

	void ItemRemoved(const uint8 EquipmentSlotLoc);
	
	
};
