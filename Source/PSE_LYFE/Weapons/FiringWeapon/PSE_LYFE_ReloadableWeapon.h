// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/FiringWeapon/PSE_LYFE_BaseFiringWeapon.h"
#include "PSE_LYFE_ReloadableWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_ReloadableWeapon : public APSE_LYFE_BaseFiringWeapon
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_ReloadableWeapon();

	void PostInitializeComponents() override;

	FTimerHandle ClientReloadTimeHandler;

	FTimerHandle ServerReloadTimeHandler;

	virtual void StartReload();

	virtual void EndReload();

	UFUNCTION(reliable, server, WithValidation)
		virtual void ServerStartReload();
	virtual bool ServerStartReload_Validate();
	virtual void ServerStartReload_Implementation();

	virtual void ServerFinishReload();

	virtual void CancelReload();

	UFUNCTION(reliable, server, WithValidation)
		virtual void ServerCancelReload();
	virtual bool ServerCancelReload_Validate();
	virtual void ServerCancelReload_Implementation();


	virtual bool AmmoCheck();//Called in both server and client

	/** Checks the ammo and returns if can reload or not */
	virtual bool CanReload();

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		float ReloadingTime;

	UPROPERTY(Transient, Replicated)
		uint16 NoOfClips;

	UPROPERTY(Transient, Replicated)
		int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 AmmoPerClip;

	UPROPERTY(EditDefaultsOnly, Category = Ammo)
		TSubclassOf<class APSE_LYFE_BaseInventoryItem> AmmoClass;

	bool UseAmmoClip();

	UPROPERTY(EditDefaultsOnly, Category = Animation)
		UAnimMontage* ReloadingAnimation;

	/** Replicated variable used to set server and client reloading animation */
	UPROPERTY(ReplicatedUsing = OnRep_SetReloadAnimation)
		bool bIsReloading;

	UFUNCTION()
		virtual void OnRep_SetReloadAnimation();
	
	
};
