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

namespace EFiringType
{
	enum Type
	{
		SemiAutomatic,
		Automatic,
		Thrown,
	};
}

namespace EReloadingType
{
	enum Type
	{
		Null,
		FullClip,
	};
}

namespace EWeaponCategory
{
	enum Type
	{
		Primary,
		Secondary,
		Melee,
		Null,
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

public:

	APSE_LYFE_BaseFiringWeapon();
	
	virtual void Tick(float DeltaSeconds) override;

	/** set the weapon's owning pawn */
	virtual void SetOwningPawn(APSE_LYFE_Character4_Weapon* AOwningCharacter) override;

	//Firing Properties

	/** Time between fires (Dont use 0)*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	float FiringRate;

	/** Current State of the Weapon */
	EWeaponState::Type CurrentState;

	/** Weapon Type */
	EFiringType::Type WeaponType;

	EReloadingType::Type ReloadingType;

	//UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	EWeaponCategory::Type WeaponCategory;

	////////////////////////////////////////////////////////////
	//Firing Logic

	virtual void StartFire();

	virtual void ClientFire();

	UFUNCTION(reliable, server, WithValidation)
	virtual void ServerStartFire();
	virtual bool ServerStartFire_Validate();
	virtual void ServerStartFire_Implementation();

	virtual void StopFire();

	UFUNCTION(reliable, server, WithValidation)
	virtual void ServerStopFire();
	virtual bool ServerStopFire_Validate();
	virtual void ServerStopFire_Implementation();

	virtual void Fire();

	virtual bool PreFireChecks();

	////////////////////////////////////////////////////////////
	//Firing Functions

	virtual FName GetFiringStartLoc();

	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;

	////////////////////////////////////////////////////////////
	//Firing Effects

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* FireAnimation;

	float PlayWeaponAnimation(UAnimMontage* Animation, float PlayRate = 1);

	void StopWeaponAnimation(UAnimMontage* Animation);

	UPROPERTY(ReplicatedUsing = OnRep_UpdateClientFireCounter)
	int8 FireCounter;

	UFUNCTION()
	void OnRep_UpdateClientFireCounter();

	/** spawn effects for impact */
	UFUNCTION(NetMulticast, Unreliable)
	void SpawnImpactEffects(const FHitResult& Impact);
	void SpawnImpactEffects_Implementation(const FHitResult& Impact);

	/** impact effects */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	TSubclassOf<class APSE_LYFE_HitImpact> ImpactTemplate;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* MuzzleFX;

	/** spawned component for muzzle FX */
	UPROPERTY()
	UParticleSystemComponent* MuzzlePSC;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	USoundCue* FiringSound;

	void PlayMuzzleFlash();

	////////////////////////////////////////////////////////////
	//Weapon Recoil

	/** Should the weapon recoil or not */
	bool bCanRecoil;

	UPROPERTY(EditDefaultsOnly, Category = Recoil)
	FRecoilProperties Recoil;

	FVector CalcRecoilDirection(FVector OriginalDirection);

	void DoRecoil();


};
