// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PSE_LYFE_BaseWeapon.h"
#include "PSE_LYFE_ReloadableWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_ReloadableWeapon : public APSE_LYFE_BaseWeapon
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_ReloadableWeapon(const FObjectInitializer& ObjectInitializer);

	void PostInitializeComponents() override;

	virtual void StartReload();

	virtual void FinishReload();

	UFUNCTION(reliable, server, WithValidation)
	virtual void ServerStartReload();

	virtual void ServerEndReload();

	virtual void CancelReload();

	UFUNCTION(reliable, server, WithValidation)
	virtual void ServerCancelReload();

	virtual bool AmmoCheck();//Called in both server and client

	/** Checks the ammo and returns if can reload or not */
	virtual bool CanReload();

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	float ReloadingTime;

	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmoInClip;

	UPROPERTY(Transient, Replicated)
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 MaxAmmoInClip;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
	int32 MaxAmmo;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* ReloadingAnimation;

	/** Replicated variable used to set server and client reloading animation */
	UPROPERTY(ReplicatedUsing = OnRep_SetReloadAnimation)
	bool bIsReloading;

	UFUNCTION()
	virtual void OnRep_SetReloadAnimation();
	
};
