// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PSE_LYFE_Character.h"
#include "Weapons/BaseFiles/PSE_LYFE_BaseWeapon.h"
#include "Weapons/BaseFiles/PSE_LYFE_GrenadeComponent.h"
#include "PSE_LYFE_ArmedCharacter.generated.h"


/** Passive weapon inventory to store weapons used for defualt inventories and such*/
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
	{}
};

/** Structure used to hold the actual pointer to the weapons */
USTRUCT()
struct FArmedWeapons
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	APSE_LYFE_BaseWeapon* PrimaryWeapon;

	UPROPERTY()
	APSE_LYFE_BaseWeapon* SecondaryWeapon;

	UPROPERTY()
	APSE_LYFE_BaseWeapon* MeleeWeapon;

	FArmedWeapons()
	{
		PrimaryWeapon = nullptr;
		SecondaryWeapon = nullptr;
		MeleeWeapon = nullptr;
	}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_ArmedCharacter : public APSE_LYFE_Character
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_ArmedCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

protected:
	/* [Server] */
	void SpawnDefaultWeapon();

public:

	/** Called by the weapon class to start fire */
	UFUNCTION()
	void StartWeaponFire();

	UFUNCTION()
	void StartWeaponReload();

	UFUNCTION()
	void StopWeaponFire();

	bool bWantsToFire;

	bool CanFire();

	//////////////////////////////////////////////////////////////////////////
	// Equip and Unequip

	UPROPERTY(EditDefaultsOnly, Category = Weapon)
	FWeaponInventory DefaultInventory;

	UPROPERTY(Transient, Replicated, RepRetry)
	FArmedWeapons CurrentArmedWeapons;

	/** Index of the armed weapon 1 = primary, 2 = secondary, 3 = melee */
	UPROPERTY(Transient, Replicated)
	uint8 CurrentWeaponIndex;

	UPROPERTY(Transient, Replicated)
	APSE_LYFE_BaseWeapon* LastUsedWeapon;

	APSE_LYFE_BaseWeapon* GetCurrentWeapon();

	APSE_LYFE_BaseWeapon* GetWeaponWithIndex(uint8 Index);

	/** Called on clients calls its server function */
	void ChangeWeaponTo(uint8 NewWeaponIndex);

	UFUNCTION(reliable, server, WithValidation)
	void ServerChangeWeaponTo(uint8 NewWeaponIndex);

	/** Equips a new weapon. Change the index before calling this(Dont call this directly) */
	void EquipWeapon(uint8 NewWeaponIndex);

	//////////////////////////////////////////////////////////////////////////
	// Character Stats

	/** Current Stamina that is replicated */
	UPROPERTY(Replicated)
	int8 CurrentStaminaRep;

	/** This holds the actual value of stamina it is decreased to int8 just for replication purposes */
	float CurrentStamina;

	/** Current Stamina */
	int8 MaxStamina;

	/** Stamina increase per second */
	float StaminaRegenRate;

	//////////////////////////////////////////////////////////////////////////
	// Input Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Grenade)
	class UPSE_LYFE_GrenadeComponent* GrenadeComp;

	void GrenadeThrowPressed();

	void GrenadeThrowReleased();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	void LeftClickPressed();

	void LeftClickReleased();

	void ChangeWeaponTo1();

	void ChangeWeaponTo2();
	
	
};
