// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_BaseWeapon.generated.h"

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

class APSE_LYFE_ArmedCharacter;

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseWeapon : public AActor
{
	GENERATED_BODY()

public:

	APSE_LYFE_BaseWeapon(const FObjectInitializer& ObjectInitializer);

	void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** set the weapon's owning pawn */
	void SetOwningPawn(APSE_LYFE_ArmedCharacter* AShooterCharacter);

	/** pawn owner */
	UPROPERTY(Transient, Replicated)
		APSE_LYFE_ArmedCharacter* MyPawn;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		UStaticMeshComponent* Mesh3P;

	////////////////////////////////////////////////////////////
	//EQuiping Logic and Properties


	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
		float EquippingTime;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
		float UnEquippingTime;

	/** weapon is being equipped by owner pawn */
	virtual void StartEquip();

	/** weapon is finish being equipped by owner pawn */
	virtual void FinishEquip();

	UFUNCTION(NetMulticast, Unreliable)
	virtual void ClientStartEquip();
	virtual void ClientStartEquip_Implementation();

	virtual void StartUnEquip();

	virtual void FinishUnEquip();

	UFUNCTION(NetMulticast, Unreliable)
	virtual void ClientStartUnEquip();
	virtual void ClientStartUnEquip_Implementation();


	UPROPERTY(ReplicatedUsing = OnRep_WeaponEquipReplication)
		bool bIsWeaponEquiped;

	UFUNCTION()
		void OnRep_WeaponEquipReplication();

	////////////////////////////////////////////////////////////
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

	////////////////////////////////////////////////////////////
	//Weapon Recoil

	/** Should the weapon recoil or not */
	bool bCanRecoil;

	UPROPERTY(EditDefaultsOnly, Category = Recoil)
		FRecoilProperties Recoil;

	FVector CalcRecoilDirection(FVector OriginalDirection);

	void DoRecoil();

	////////////////////////////////////////////////////////////
	//Misc

	void TestState(FColor Colour = FColor::Cyan);

protected:
	/** Returns Mesh3P subobject **/
	FORCEINLINE UStaticMeshComponent* GetMesh3P() const { return Mesh3P; }


};
