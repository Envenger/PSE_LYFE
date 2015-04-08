// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_ReloadableWeapon.h"
#include "UnrealNetwork.h"

APSE_LYFE_ReloadableWeapon::APSE_LYFE_ReloadableWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MaxAmmoInClip = 5;
	MaxAmmo = 25;
	ReloadingTime = 2.17f;
	ReloadingType = EReloadingType::FullClip;

	bIsReloading = false;
}

void APSE_LYFE_ReloadableWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CurrentAmmo = MaxAmmo;
	CurrentAmmoInClip = MaxAmmoInClip;
}

void APSE_LYFE_ReloadableWeapon::StartReload()
{
	if (CurrentState == EWeaponState::Idle)
	{
		CurrentState = EWeaponState::Reloading;
		GetWorldTimerManager().SetTimer(ClientReloadTimeHandler, this, &APSE_LYFE_ReloadableWeapon::ClientEndReload, ReloadingTime, false);
		PlayWeaponAnimation(ReloadingAnimation);
		ServerStartReload();
	}
}


void APSE_LYFE_ReloadableWeapon::ClientEndReload()
{
	CurrentState = EWeaponState::Idle;
	StopWeaponAnimation(ReloadingAnimation);
}

bool APSE_LYFE_ReloadableWeapon::ServerStartReload_Validate()
{
	return true;
}

void APSE_LYFE_ReloadableWeapon::ServerStartReload_Implementation()
{
	if (CurrentState == EWeaponState::Idle)
	{
		CurrentState = EWeaponState::Reloading;
		GetWorldTimerManager().SetTimer(ServerReloadTimeHandler, this, &APSE_LYFE_ReloadableWeapon::ServerFinishReload, (ReloadingTime - 0.1f), false);
		bIsReloading = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Can only reload in idel state client");
	}
}

void APSE_LYFE_ReloadableWeapon::ServerFinishReload()
{
	CurrentState = EWeaponState::Idle;
	CurrentAmmoInClip = MaxAmmoInClip;
	CurrentAmmo -= MaxAmmoInClip;
	bIsReloading = false;
}

void APSE_LYFE_ReloadableWeapon::CancelReload()
{
	CurrentState = EWeaponState::Idle;
	GetWorldTimerManager().ClearTimer(ClientReloadTimeHandler);
	StopWeaponAnimation(ReloadingAnimation);
	ServerCancelReload();
}

bool APSE_LYFE_ReloadableWeapon::ServerCancelReload_Validate()
{
	return true;
}

void APSE_LYFE_ReloadableWeapon::ServerCancelReload_Implementation()
{
	GetWorldTimerManager().ClearTimer(ServerReloadTimeHandler);
	CurrentState = EWeaponState::Idle;
	bIsReloading = false;
}

bool APSE_LYFE_ReloadableWeapon::AmmoCheck()
{
	if (CurrentAmmoInClip > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool APSE_LYFE_ReloadableWeapon::CanReload()
{
	if ((CurrentAmmo >= MaxAmmoInClip) && (CurrentAmmoInClip < MaxAmmoInClip) && (CurrentAmmo > 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void APSE_LYFE_ReloadableWeapon::OnRep_SetReloadAnimation()
{
	if (MyPawn)
		if (bIsReloading == true)
		{
			PlayWeaponAnimation(ReloadingAnimation);
		}
		else
		{
			StopWeaponAnimation(ReloadingAnimation);
		}
}

void APSE_LYFE_ReloadableWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_ReloadableWeapon, bIsReloading, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(APSE_LYFE_ReloadableWeapon, CurrentAmmoInClip, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_ReloadableWeapon, CurrentAmmo, COND_OwnerOnly);
}


