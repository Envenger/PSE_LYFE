// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "PSE_LYFE_AutoRWeapon.h"


APSE_LYFE_AutoRWeapon::APSE_LYFE_AutoRWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponType = EFiringType::Automatic; // Can only fire when the weapon is idle
	FiringRate = 0.15;

	LastFiringTime = FiringRate;
	DelayedFireStart = false;

	AmmoPerClip = 30;

	//Recoil
	Recoil.MaxRecoil = 60;
	Recoil.RecoilPerShot = 12;
	Recoil.RecoilResetRate = 50;
	Recoil.StaminaInfluenceCoeff = 0.4;
	Recoil.StaminaDrain = 8;
}

void APSE_LYFE_AutoRWeapon::StartFire()
{
	RepeatingStartFire();
	GetWorldTimerManager().SetTimer(RepeatingStartFireTimerHandle, this, &APSE_LYFE_AutoRWeapon::RepeatingStartFire, FiringRate, true);
}

void APSE_LYFE_AutoRWeapon::RepeatingStartFire()
{
	if (PreFireChecks())
	{
		float CurrentTime = GetWorld()->GetRealTimeSeconds();
		float TimeGap = CurrentTime - LastFiringTime;
		if (TimeGap > FiringRate)
		{
			ClientCallStartFire();
		}
		else
		{
			if (DelayedFireStart == false)
			{
				GetWorldTimerManager().SetTimer(ClientCallFireHandle, this, &APSE_LYFE_AutoRWeapon::ClientCallStartFire, (FiringRate - TimeGap), false);
				DelayedFireStart = true;
			}
		}
	}
}

void APSE_LYFE_AutoRWeapon::ClientCallStartFire()
{
	ServerStartFire();
	StartRepeatingClientFire();
	if (DelayedFireStart == true)
	{
		DelayedFireStart = false;
	}
}

void APSE_LYFE_AutoRWeapon::StartRepeatingClientFire()
{
	CurrentState = EWeaponState::Firing;
	ClientFire();
	GetWorldTimerManager().SetTimer(ClientFireHandle, this, &APSE_LYFE_AutoRWeapon::ClientFire, FiringRate, true);
}

void APSE_LYFE_AutoRWeapon::ClientFire()
{
	if (AmmoCheck())
	{
		if (CurrentState == EWeaponState::Firing)
		{
			PlayWeaponAnimation(FireAnimation);
			LastFiringTime = GetWorld()->GetRealTimeSeconds();
		}
	}
	else
	{
		ClientCallStopFire();
	}
}

bool APSE_LYFE_AutoRWeapon::ServerStartFire_Validate()
{
	return true;
}

void APSE_LYFE_AutoRWeapon::ServerStartFire_Implementation()
{
	float CurrentTime = GetWorld()->GetRealTimeSeconds();
	float TimeGap = CurrentTime - LastFiringTime;
	if (TimeGap > FiringRate)
	{
		ServerCallStartFire();
	}
	else if (DelayedFireStart == false)
	{
		DelayedFireStart = true;
		GetWorldTimerManager().SetTimer(ServerCallFireHandle, this, &APSE_LYFE_AutoRWeapon::ServerCallStartFire, (FiringRate - TimeGap), false);
	}
}

void APSE_LYFE_AutoRWeapon::ServerCallStartFire()
{
	if (PreFireChecks())
	{
		CurrentState = EWeaponState::Firing;
		Fire();
		GetWorldTimerManager().SetTimer(ServerFireTimerHandle, this, &APSE_LYFE_AutoRWeapon::Fire, FiringRate, true);
	}
	if (DelayedFireStart == true)
	{
		DelayedFireStart = false;
	}
}


void APSE_LYFE_AutoRWeapon::Fire()
{
	if (Role == ROLE_Authority)
	{
		if (AmmoCheck())
		{
			if (CurrentState == EWeaponState::Firing)
			{
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

				DrawDebugLine(GetWorld(), WeaponTraceStart, WeaponTraceEnd, FColor(255, 0, 0, 1), true);

				SpawnImpactEffects(Impact);

				LastFiringTime = GetWorld()->GetRealTimeSeconds();
				if (Impact.GetActor())
				{
					if (Impact.GetComponent()->Mobility == EComponentMobility::Movable && Impact.GetComponent()->IsSimulatingPhysics())
					{
						Impact.GetComponent()->AddImpulse(WeaponTraceDirection * 60000);
						GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, Impact.GetActor()->GetName());
					
						//UPrimitiveComponent* Object = Cast<UPrimitiveComponent>(Impact.GetActor()->GetRootComponent());
						//Object->AddImpulse(WeaponTraceDirection * 6000000);
					}
				}
				FireCounter++;
				CurrentAmmo -= 1;
				LastFiringTime = GetWorld()->GetRealTimeSeconds();
				DoRecoil();
			}
			else
			{
				GetWorldTimerManager().ClearTimer(ServerFireTimerHandle);
			}
		}
		else
		{
			GetWorldTimerManager().ClearTimer(ServerFireTimerHandle);
		}
	}
}

void APSE_LYFE_AutoRWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(RepeatingStartFireTimerHandle);
	ClientCallStopFire();
}

void APSE_LYFE_AutoRWeapon::ClientCallStopFire()
{
	StopWeaponAnimation(FireAnimation);
	ServerStopFire();
	if (GetWorldTimerManager().IsTimerActive(ClientCallFireHandle))
	{
		GetWorldTimerManager().ClearTimer(ClientCallFireHandle);
	}
	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
		GetWorldTimerManager().ClearTimer(ClientFireHandle);
		if (DelayedFireStart == true)
		{
			GetWorldTimerManager().ClearTimer(ClientCallFireHandle);
		}
	}
}

bool APSE_LYFE_AutoRWeapon::ServerStopFire_Validate()
{
	return true;
}

void APSE_LYFE_AutoRWeapon::ServerStopFire_Implementation()
{
	FireCounter = 0;
	if (GetWorldTimerManager().IsTimerActive(ServerCallFireHandle))
	{
		GetWorldTimerManager().ClearTimer(ServerCallFireHandle);
	}
	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
	}
}

void APSE_LYFE_AutoRWeapon::StartReload()
{
	ClientCallStopFire();
	Super::StartReload();
}


bool APSE_LYFE_AutoRWeapon::PreFireChecks() // Firing not started yet so no need to stop it before reloading
{
	if (CurrentState == EWeaponState::Idle)
	{
		if (AmmoCheck())
		{
			return true;
		}
		else
		{
			if (CanReload())
			{
				StartReload();
			}
			return false;
		}
	}
	return false;
}


