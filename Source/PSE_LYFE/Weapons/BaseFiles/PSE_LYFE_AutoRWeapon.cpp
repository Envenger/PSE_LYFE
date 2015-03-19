// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/PSE_LYFE_ArmedCharacter.h"
#include "PSE_LYFE_AutoRWeapon.h"


APSE_LYFE_AutoRWeapon::APSE_LYFE_AutoRWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponType = EFiringType::Automatic; // Can only fire when the weapon is idle
	FiringRate = 0.15;

	LastFiringTime = FiringRate;
	DelayedFireStart = false;

	MaxAmmoInClip = 30;
	MaxAmmo = 300;

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
	GetWorldTimerManager().SetTimer(this, &APSE_LYFE_AutoRWeapon::RepeatingStartFire, FiringRate, true);
}

void APSE_LYFE_AutoRWeapon::RepeatingStartFire()
{
	if (PreFireChecks())
	{
		float CurrentTime = GetWorld()->GetRealTimeSeconds();
		float TimeGap = CurrentTime - LastFiringTime;
		if (TimeGap > FiringRate)
		{
			CallStartFire();
		}
		else
		{
			if (DelayedFireStart == false)
			{
				GetWorldTimerManager().SetTimer(this, &APSE_LYFE_AutoRWeapon::CallStartFire, (FiringRate - TimeGap), false);
				DelayedFireStart = true;
			}
		}
	}
}

void APSE_LYFE_AutoRWeapon::CallStartFire()
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
	GetWorldTimerManager().SetTimer(this, &APSE_LYFE_AutoRWeapon::ClientFire, FiringRate, true);
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
		CallStopFire();
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
		GetWorldTimerManager().SetTimer(this, &APSE_LYFE_AutoRWeapon::ServerCallStartFire, (FiringRate - TimeGap), false);
	}
}

void APSE_LYFE_AutoRWeapon::ServerCallStartFire()
{
	if (PreFireChecks())
	{
		CurrentState = EWeaponState::Firing;
		Fire();
		GetWorldTimerManager().SetTimer(this, &APSE_LYFE_AutoRWeapon::Fire, FiringRate, true);
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

				//

				//float Angle = FMath::Acos(RecoiledDirection.DotProduct(RecoiledDirection, WeaponTraceDirection));
				//GEngine->AddOnScreenDebugMessage(-1, 1.2f, FColor::Cyan, FString::SanitizeFloat(FMath::RadiansToDegrees(Angle)));

				//	FVector WeaponTraceEnd = WeaponTraceStart + (30000 * WeaponTraceDirection);


				SpawnImpactEffects(Impact);

				LastFiringTime = GetWorld()->GetRealTimeSeconds();
				if (Impact.GetActor())
				{
					if (Impact.GetActor()->GetRootComponent())
					{
						UPrimitiveComponent* Object = Cast<UPrimitiveComponent>(Impact.GetActor()->GetRootComponent());
						Object->AddImpulse(WeaponTraceDirection * 60000);
					}
				}
				FireCounter++;
				CurrentAmmoInClip -= 1;
				LastFiringTime = GetWorld()->GetRealTimeSeconds();
				DoRecoil();
			}
			else
			{
				GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::Fire);
			}
		}
		else
		{
			GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::Fire);
		}
	}
}

void APSE_LYFE_AutoRWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::RepeatingStartFire);
	CallStopFire();
}

void APSE_LYFE_AutoRWeapon::CallStopFire()
{
	StopWeaponAnimation(FireAnimation);
	ServerStopFire();
	if (GetWorldTimerManager().IsTimerActive(this, &APSE_LYFE_AutoRWeapon::CallStartFire))
	{
		GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::CallStartFire);
	}
	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
		GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::ClientFire);
		if (DelayedFireStart == true)
		{
			GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::CallStartFire);
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
	if (GetWorldTimerManager().IsTimerActive(this, &APSE_LYFE_AutoRWeapon::ServerCallStartFire))
	{
		GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::ServerCallStartFire);
	}
	if (CurrentState == EWeaponState::Firing)
	{
		CurrentState = EWeaponState::Idle;
	}
	if (DelayedFireStart == true)
	{
		GetWorldTimerManager().ClearTimer(this, &APSE_LYFE_AutoRWeapon::CallStartFire);
	}
}

void APSE_LYFE_AutoRWeapon::StartReload()
{
	CallStopFire();
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


