// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PSE_LYFE_ReloadableWeapon.h"
#include "PSE_LYFE_AutoRWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_AutoRWeapon : public APSE_LYFE_ReloadableWeapon
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_AutoRWeapon(const FObjectInitializer& ObjectInitializer);

	virtual void StartFire() override;

	virtual void RepeatingStartFire();

	virtual void CallStartFire();

	virtual void StartRepeatingClientFire();

	virtual void ClientFire() override;

	virtual bool ServerStartFire_Validate() override;

	virtual void ServerStartFire_Implementation() override;

	virtual void ServerCallStartFire();// Starts repeating server fire..

	float LastFiringTime;

	bool DelayedFireStart;

	virtual void Fire() override;

	virtual void StopFire() override;

	virtual void CallStopFire();

	virtual bool ServerStopFire_Validate() override;

	virtual void ServerStopFire_Implementation() override;

	virtual bool PreFireChecks() override;

	virtual void StartReload() override;
	
	
};
