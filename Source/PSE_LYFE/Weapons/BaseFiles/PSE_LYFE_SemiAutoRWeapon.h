// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PSE_LYFE_ReloadableWeapon.h"
#include "PSE_LYFE_SemiAutoRWeapon.generated.h"

/**
*
*/
UCLASS()
class PSE_LYFE_API APSE_LYFE_SemiAutoRWeapon : public APSE_LYFE_ReloadableWeapon
{
	GENERATED_BODY()

public:

	APSE_LYFE_SemiAutoRWeapon(const FObjectInitializer& ObjectInitializer);

	/** Only used by the server for how many shots to fire calculations */
	bool bServerExtendFire;

	bool PreFireChecks();//Added extra Ammo Check and reload funcitonality

	/** Timer used only for Semi-Automatic weapons */
	virtual void ClientResetTimer();

	virtual void StartFire() override;

	virtual bool ServerStartFire_Validate() override;

	virtual void ServerStartFire_Implementation() override;

	virtual void ServerResetTimer();

	virtual void ClientFire() override;

	virtual void Fire() override;

	float LastFiringTime;

	virtual void StartReload() override;

	/** Will reset the firing state but wont reset the fire cooldown */
	virtual void StopFire() override;

	virtual bool ServerStopFire_Validate() override;

	/** Will reset the firing state but wont reset the fire cooldown */
	virtual void ServerStopFire_Implementation() override;

	/** When the semiautomatic cool down goes over */
	bool ReadyToFire;


};
