// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/BaseFiles/PSE_LYFE_BaseWeapon.h"
#include "PSE_LYFE_BaseFiringWeapon.generated.h"

namespace EWeaponState
{
	enum Type
	{
		Null, //Weapon
		Passive, //Weapon present with the player but in passive state
		Equipping, //Weapon being currently equiping
		Idle, //Weapon Equiuped but not in any state
		ReadyToFire, //Used only in case of thrown weapons
		Firing, // Weapon currently firing
		Reloading, //Weapon is currnetly reloading
	};
}

USTRUCT()
struct FRecoilProperties
{
	GENERATED_USTRUCT_BODY()

	/** Max Recoil Value at value of 200 shot gets fired(at at 45 degree change it in CalcRecoilDirection)*/
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float MaxRecoil;

	/** Current Recoil Value */
	UPROPERTY()
	float CurrentRecoil;

	/** Increase of recoil per shot */
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float RecoilPerShot;

	/** How much value of the recoil is reset each second when not firing */
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float RecoilResetRate;

	/** Multiplier of the (MaxStamina - CurrentStamina) used for recoil */
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float StaminaInfluenceCoeff;

	/** How much stamina does 1 shot drain */
	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	float StaminaDrain;

	void DoRecoil()
	{
		float RecoilIncrease = FMath::Min((MaxRecoil - CurrentRecoil), RecoilPerShot);
		CurrentRecoil += RecoilIncrease;
	}

	FRecoilProperties()
	{
		MaxRecoil = 40;
		CurrentRecoil = 0;
		RecoilPerShot = 10;
		RecoilResetRate = 55;
		StaminaInfluenceCoeff = 0.3;
		StaminaDrain = 8;
	}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseFiringWeapon : public APSE_LYFE_BaseWeapon
{
	GENERATED_BODY()
	
	
	
	
};
