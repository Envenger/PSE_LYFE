// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Inventory/PSE_LYFE_Inventory2_Storage.h"
#include "PSE_LYFE_Inventory3_Equipment.generated.h"


UCLASS()
class PSE_LYFE_API APSE_LYFE_Inventory3_Equipment : public APSE_LYFE_Inventory2_Storage
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_Inventory3_Equipment();

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing = OnRep_EquipmentChanged)
	TArray<FItemStruct> EquipmentStorage;

	/** Function that gets replicated on all clients when a character equipment slot changes */
	UFUNCTION()
	void OnRep_EquipmentChanged();

	/** Client version of the equipments that get updated through RepNotifies */
	TArray<FItemStruct> ClientEquipmentStorage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = EquipmentSlots)
	TArray<EEquipmentSlotType> EquipmentSlots;

	/** Takes the equipment storage and initializes the storage base */
	void InitializeStorage();

	bool CheckSlotType(const uint8 EquipmentSlotLoc, const EEquipmentSlotType EquipmentSlotType) const;

	const bool AddBackPack(FItemStruct &NewBackPackItemStruct);

	const bool RemoveBackPack();

	const bool AddWeapon(FItemStruct &WeaponItem, const uint8 EquipmentSlotLoc);

	const bool RemoveWeapon(const uint8 EquipmentSlotLoc);

	const bool ProcessAddInventoryEquipment(FItemStruct &NewItem, const uint8 EquipmentSlotLoc);

	const bool ProcessRemoveInventoryEquipment(const uint8 EquipmentSlotLoc);

	/** Item slot type needs to be checked before adding item or it wont work. Replaces the  */
	void AddInventoryEquipment(FItemStruct &NewItem, const uint8 EquipmentSlotLoc);

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

	void EquipmentItemAdded(const uint8 EquipmentSlotLoc);

	void EquipmentItemRemoved(const uint8 EquipmentSlotLoc);
	
	
};
