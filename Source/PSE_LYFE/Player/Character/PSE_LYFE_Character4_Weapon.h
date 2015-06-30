// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Character/PSE_LYFE_Character3_Survival.h"
#include "Weapons/BaseFiles/PSE_LYFE_BaseWeapon.h"
#include "Weapons/BaseFiles/PSE_LYFE_GrenadeComponent.h"
#include "Items/Weapon/PSE_LYFE_BaseWeaponItem.h"
#include "PSE_LYFE_Character4_Weapon.generated.h"

/** Passive weapon inventory to store weapons used for default inventories and such*/
USTRUCT()
struct FWeaponInventory
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<APSE_LYFE_BaseWeapon> PrimaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<APSE_LYFE_BaseWeapon> SecondaryWeapon;

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	TSubclassOf<APSE_LYFE_BaseWeapon> MeleeWeapon;

	FWeaponInventory()
	{
		PrimaryWeapon = nullptr;
		SecondaryWeapon = nullptr;
		MeleeWeapon = nullptr;
	}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_Character4_Weapon : public APSE_LYFE_Character3_Survival
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_Character4_Weapon();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetupPlayerInputComponent(class UInputComponent* InputComponent);

	virtual void LeftClickPressed();

	bool bIsLeftClickPressed;

	virtual void LeftClickReleased();

public:

	/** Called by the weapon class to start fire */
	UFUNCTION()
	void StartWeaponFire();

	UFUNCTION()
	void StartWeaponReload();

	void CancleWeaponReload();

	UFUNCTION()
	void StopWeaponFire();

	bool bWantsToFire;

	bool CanFire();

//////////////////////////////////////////////////////////////////////////
// Equip and UnEquip

protected:
	/* [Server] */
	void SpawnDefaultWeapon();

	bool SpawnWeapon(TSubclassOf<APSE_LYFE_BaseWeapon> WeaponClass, int8 WeaponSlotIndex);

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FWeaponInventory DefaultInventory;

	UPROPERTY(ReplicatedUsing = OnRep_ArmedWeapons)
	TArray<APSE_LYFE_BaseWeapon*> CurrentArmedWeapons;

	/** If the player goes network irrelevant and weapon is changed when it again becomes relevant, OnRepArmedWeaponChange won work */
	UFUNCTION()
	void OnRep_ArmedWeapons();

	/** Keeps a copy of the server weapons used for OnReArmedWeaponChange function */
	UPROPERTY()
	TArray<APSE_LYFE_BaseWeapon*> ClientCurrentArmedWeapons;

	/** Used when the player already has a weapon and changed in that slot */
	UPROPERTY(Replicated)
	APSE_LYFE_BaseWeapon* TemporaryWeapon;

	/** Index of the armed weapon 0 = primary, 1 = secondary, 2 = melee */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeaponIndex)
	int8 CurrentWeaponIndex;

	UFUNCTION()
	void OnRep_CurrentWeaponIndex();

	UPROPERTY(Transient)
	APSE_LYFE_BaseWeapon* OwnerLastUsedWeapon;

public:
	APSE_LYFE_BaseWeapon* GetCurrentWeapon() const;

protected:
	APSE_LYFE_BaseWeapon* GetWeaponWithIndex(int8 Index) const;

	/** Called on clients calls its server function */
	void ChangeWeaponTo(int8 NewWeaponIndex);

	UFUNCTION(reliable, server, WithValidation)
	void ServerChangeWeaponTo(int8 NewWeaponIndex);
	bool ServerChangeWeaponTo_Validate(int8 NewWeaponIndex);
	void ServerChangeWeaponTo_Implementation(int8 NewWeaponIndex);

	/** Called when the weapon change is finished on server */
	void ChangeWeaponFinish(int8 NewWeaponIndex);

	/** Sets the weapon and is replicated to clients[server] */
	void SetCurrentWeapon(int8 NewWeaponIndex);

	void ReplaceRemoveCurrentWeapon();

	UFUNCTION(NetMulticast, Unreliable)
	void Client_UnEquipWeapon(int8 WeaponIndex, int8 NewWeaponIndex = -1);
	void Client_UnEquipWeapon_Implementation(int8 WeaponIndex, int8 NewWeaponIndex = -1);

	UFUNCTION(NetMulticast, Unreliable)
	void Client_EquipWeapon(int8 WeaponIndex);
	void Client_EquipWeapon_Implementation(int8 WeaponIndex);

	void StartUnEquipAnimation(int8 WeaponIndex, int8 NewWeaponIndex = -1);

private:

	int8 AnimBP_UnEquipSlot;

public:

	UFUNCTION(BlueprintCallable, Category = AnimBP)
	void AnimBP_UnEquipNotify();

protected:

	void FinishUnEquipAnimation(int8 WeaponIndex, int8 NewWeaponIndex = -1);

	void StartEquipAnimation(int8 WeaponIndex);

private:

	int8 AnimBP_EquipSlot;

public:

	UFUNCTION(BlueprintCallable, Category = AnimBP)
	void AnimBP_EquipNotify();

protected:

	void FinishEquipAnimation();

	const int8 GetNextWeapon(const int8 CurrentRemovedWeapon) const;
public:

	/** [Server] True when weapon is changing also used optionally with clients to lower network bandwidth*/
	bool bIsWeaponChanging;

	void PlayerAddWeapon(TSubclassOf<class APSE_LYFE_BaseWeapon> WeaponClass, const uint8 WeaponSlot);

	void PlayerRemoveWeapon(const int8 WeaponSlot);

protected:

	void DeSpawnWeapon(const int8 WeaponSlot, int8 NewWeaponIndex = -1);

////////////////////////////////////////////////////
//Hand Ik

protected:

	FVector LeftHandWeaponAttachmentLocation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimBP)
	bool AnimBP_bIKLeftHand;

	void SetWeaponIKDetection(const bool bIKDetection);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimBP)
	FVector AnimBP_HandAttachmentLocation;

////////////////////////////////////////////////////
//Grenade

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grenade)
	class UPSE_LYFE_GrenadeComponent* GrenadeComp;

	void GrenadeThrowPressed();

	void GrenadeThrowReleased();

protected:

	void ChangeWeaponTo1();

	void ChangeWeaponTo2();
	
public:

	const TArray<APSE_LYFE_BaseWeapon*> GetAllWeapons() const;
};
