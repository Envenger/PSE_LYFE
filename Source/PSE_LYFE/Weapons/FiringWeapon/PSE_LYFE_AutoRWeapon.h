// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/FiringWeapon/PSE_LYFE_ReloadableWeapon.h"
#include "PSE_LYFE_AutoRWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_AutoRWeapon : public APSE_LYFE_ReloadableWeapon
{
	GENERATED_BODY()

	APSE_LYFE_AutoRWeapon();

	virtual void StartFire() override;

	FTimerHandle RepeatingStartFireTimerHandle;

	/** A repeating counter that keeps getting called when the fire button is pressed */
	virtual void RepeatingStartFire();

	FTimerHandle ClientCallFireHandle;

	virtual void ClientCallStartFire();

	virtual void StartRepeatingClientFire();

	FTimerHandle ClientFireHandle;

	virtual void ClientFire() override;

	virtual bool ServerStartFire_Validate() override;

	virtual void ServerStartFire_Implementation() override;

	FTimerHandle ServerCallFireHandle;

	virtual void ServerCallStartFire();// Starts repeating server fire..

	float LastFiringTime;

	bool DelayedFireStart;

	FTimerHandle ServerFireTimerHandle;

	virtual void Fire() override;

	virtual void StopFire() override;

	virtual void ClientCallStopFire();

	virtual bool ServerStopFire_Validate() override;

	virtual void ServerStopFire_Implementation() override;

	virtual bool PreFireChecks() override;

	virtual void StartReload() override;
	
	
};
