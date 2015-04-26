// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_SemiAutoRWeapon.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "UnrealNetwork.h"

APSE_LYFE_SemiAutoRWeapon::APSE_LYFE_SemiAutoRWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponType = EFiringType::SemiAutomatic; // Can only fire when the weapon is idle
	bServerExtendFire = false;
	FiringRate = 0.75;
	LastFiringTime = FiringRate;

	ReadyToFire = true;

	//Recoil
	Recoil.MaxRecoil = 40;
	Recoil.RecoilPerShot = 10;
	Recoil.RecoilResetRate = 30;
	Recoil.StaminaInfluenceCoeff = 0.3;
	Recoil.StaminaDrain = 12;
}

void APSE_LYFE_SemiAutoRWeapon::StartFire()
{
	if (PreFireChecks())
	{
		ClientFire();
		ServerStartFire();
	}
}

void APSE_LYFE_SemiAutoRWeapon::ClientFire()
{
	PlayWeaponAnimation(FireAnimation);
	CurrentState = EWeaponState::Firing;
	ReadyToFire = false;
	FTimerHandle ClientFireResetHandle;
	GetWorldTimerManager().SetTimer(ClientFireResetHandle, this, &APSE_LYFE_SemiAutoRWeapon::ClientResetTimer, FiringRate, false);
}

void APSE_LYFE_SemiAutoRWeapon::ClientResetTimer()
{
	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
	}
	ReadyToFire = true;
}

bool APSE_LYFE_SemiAutoRWeapon::ServerStartFire_Validate()
{
	return true;
}

void APSE_LYFE_SemiAutoRWeapon::ServerStartFire_Implementation()
{
	if (AmmoCheck())
	{
		if (CurrentState == EWeaponState::Idle && ReadyToFire == true)//Ammo check
		{
			CurrentState = EWeaponState::Firing;
			Fire();
		}
		else if (CurrentState == EWeaponState::Firing)
		{
			float Now = GetWorld()->GetRealTimeSeconds();
			if ((Now - LastFiringTime) > FiringRate)
			{
				FTimerHandle ServerFireDelayHandle;
				GetWorldTimerManager().SetTimer(ServerFireDelayHandle, this, &APSE_LYFE_SemiAutoRWeapon::Fire, (Now - LastFiringTime), false);
			}
		}
	}
}

void APSE_LYFE_SemiAutoRWeapon::Fire()
{
	if (Role == ROLE_Authority)
	{
		ReadyToFire = false;
		CurrentState = EWeaponState::Firing;

		FVector StartTrace = MyPawn->ViewOrigin;
		FVector TraceDirection = MyPawn->ViewDirection;
		FVector EndTrace = StartTrace + (30000 * TraceDirection);
		const FHitResult HudImpact = WeaponTrace(StartTrace, EndTrace);// First Trace

		FVector WeaponTraceStart = Mesh3P->GetSocketLocation(GetFiringStartLoc());
		FVector WeaponTraceDirection = (HudImpact.ImpactPoint - WeaponTraceStart);
		WeaponTraceDirection.Normalize();
		FVector RecoiledDirection = CalcRecoilDirection(WeaponTraceDirection);
		FVector WeaponTraceEnd = WeaponTraceStart + (30000 * RecoiledDirection);
		const FHitResult Impact = WeaponTrace(WeaponTraceStart, WeaponTraceEnd);// 2nd Trace

		SpawnImpactEffects(Impact);

		LastFiringTime = GetWorld()->GetRealTimeSeconds();
		if (Impact.GetActor())
		{
			if (Impact.GetActor()->GetRootComponent())
			{
				UPrimitiveComponent* Object = Cast<UPrimitiveComponent>(Impact.GetActor()->GetRootComponent());
				Object->AddImpulse(WeaponTraceDirection * 180000);
			}
		}
		FireCounter++;
		CurrentAmmo -= 1;
		DoRecoil();
	}
	FTimerHandle ServerFireRestHandle;
	GetWorldTimerManager().SetTimer(ServerFireRestHandle, this, &APSE_LYFE_SemiAutoRWeapon::ServerResetTimer, FiringRate, false);//Resets fire after each fire
}

void APSE_LYFE_SemiAutoRWeapon::ServerResetTimer()
{
	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
	}
	if (FireCounter > 0)
	{
		FireCounter = 0;
	}
	ReadyToFire = true;
}

void APSE_LYFE_SemiAutoRWeapon::StartReload()
{
	StopFire();
	Super::StartReload();
}

void APSE_LYFE_SemiAutoRWeapon::StopFire()
{
	StopWeaponAnimation(FireAnimation);
	CurrentState = EWeaponState::Idle;
	ServerStopFire();
}

bool APSE_LYFE_SemiAutoRWeapon::ServerStopFire_Validate()
{
	return true;
}

void APSE_LYFE_SemiAutoRWeapon::ServerStopFire_Implementation()
{
	CurrentState = EWeaponState::Idle;
}

bool APSE_LYFE_SemiAutoRWeapon::PreFireChecks()
{
	if (AmmoCheck())
	{
		if (CurrentState == EWeaponState::Idle && ReadyToFire == true)
		{
			return true;
		}
	}
	else if (CanReload())
	{
		if (CurrentState != EWeaponState::Reloading)
		{
			StartReload();
		}
	}
	return false;
}


