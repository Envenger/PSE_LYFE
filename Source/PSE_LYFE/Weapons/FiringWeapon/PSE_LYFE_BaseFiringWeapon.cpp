// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "UnrealNetwork.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "Weapons/Effects/PSE_LYFE_HitImpact.h"
#include "PSE_LYFE_BaseFiringWeapon.h"

APSE_LYFE_BaseFiringWeapon::APSE_LYFE_BaseFiringWeapon()
{
	FiringRate = 0.1;

	CurrentState = EWeaponState::Null;
	ReloadingType = EReloadingType::Null;
	WeaponCategory = EWeaponCategory::Primary;

	bCanRecoil = true;

	FireCounter = 0;
	CurrentState = EWeaponState::Idle;
}

void APSE_LYFE_BaseFiringWeapon::SetOwningPawn(APSE_LYFE_Character4_Weapon* AOwningCharacter)
{
	Super::SetOwningPawn(AOwningCharacter);
	CurrentState = EWeaponState::Idle;
}

void APSE_LYFE_BaseFiringWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentState != EWeaponState::Firing)
	{
		if (bCanRecoil)
		{
			if (Recoil.CurrentRecoil > 0)
			{
				float RecoilDec = FMath::Min(Recoil.CurrentRecoil, (Recoil.RecoilResetRate*DeltaSeconds));
				Recoil.CurrentRecoil -= RecoilDec;// Decrease recoil with time
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void APSE_LYFE_BaseFiringWeapon::StartFire()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, "Base Weapon Fire");
	if (PreFireChecks())
	{
		if (Role < ROLE_Authority)
		{
			ServerStartFire();
		}
		Fire();
	}
}

bool APSE_LYFE_BaseFiringWeapon::ServerStartFire_Validate()
{
	return true;
}

void APSE_LYFE_BaseFiringWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

void APSE_LYFE_BaseFiringWeapon::Fire()
{
	if (Role == ROLE_Authority)
	{
		FVector StartTrace = MyPawn->ViewOrigin;
		FVector TraceDirection = MyPawn->ViewDirection;
		FVector EndTrace = StartTrace + (10000 * TraceDirection);
		const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

		if (Impact.GetActor())
		{
			if (Impact.GetActor()->GetRootComponent())
			{
				UPrimitiveComponent* Object = Cast<UPrimitiveComponent>(Impact.GetActor()->GetRootComponent());
				Object->AddImpulse(TraceDirection * 18000);
			}
		}
	}
}

bool APSE_LYFE_BaseFiringWeapon::PreFireChecks()
{
	return true;
}

void APSE_LYFE_BaseFiringWeapon::StopFire()
{

}

void APSE_LYFE_BaseFiringWeapon::ClientFire()
{
}


bool APSE_LYFE_BaseFiringWeapon::ServerStopFire_Validate()
{
	return true;
}

void APSE_LYFE_BaseFiringWeapon::ServerStopFire_Implementation()
{
	StopFire();
}


FHitResult APSE_LYFE_BaseFiringWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_PhysicsBody, TraceParams);

	return Hit;
}

FVector APSE_LYFE_BaseFiringWeapon::CalcRecoilDirection(FVector OriginalDirection)
{
	if (MyPawn)
	{
		float FinalRecoilCoeff = Recoil.CurrentRecoil + (Recoil.StaminaInfluenceCoeff * (MyPawn->MaxStamina - MyPawn->CurrentStamina));
		float RandomValue = FMath::FRandRange(0, FinalRecoilCoeff);
		float RandomAngle = FMath::FRandRange(0, 360);
		FVector RecoilVector(0, RandomValue*FMath::Cos(RandomAngle), RandomValue*FMath::Sin(RandomAngle));
		RecoilVector = RecoilVector / 200;
		FQuat OriginalDirectionQuat = OriginalDirection.Rotation().Quaternion();
		RecoilVector = OriginalDirectionQuat.RotateVector(RecoilVector);
		OriginalDirection += RecoilVector / 2; //(90/2) = 45
		OriginalDirection.Normalize();
		return OriginalDirection;
	}
	return FVector(0, 0, 0);
}

void APSE_LYFE_BaseFiringWeapon::DoRecoil()
{
	if (Role == ROLE_Authority)
	{
		float StaminaDec = FMath::Min(MyPawn->CurrentStamina, Recoil.StaminaDrain);

		MyPawn->CurrentStamina -= StaminaDec;
	}
	Recoil.DoRecoil();
}

FName APSE_LYFE_BaseFiringWeapon::GetFiringStartLoc()
{
	return FName(TEXT("FiringPoint"));
}

void APSE_LYFE_BaseFiringWeapon::OnRep_UpdateClientFireCounter()
{
	if (FireCounter > 0)
	{
		PlayWeaponAnimation(FireAnimation);
		PlayMuzzleFlash();
	}
	else
	{
		StopWeaponAnimation(FireAnimation);
	}
}

float APSE_LYFE_BaseFiringWeapon::PlayWeaponAnimation(UAnimMontage* Animation, float PlayRate)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		Duration = MyPawn->PlayAnimMontage(Animation, PlayRate);
	}
	return Duration;
}

void APSE_LYFE_BaseFiringWeapon::StopWeaponAnimation(UAnimMontage* Animation)
{
	if (MyPawn)
	{
		MyPawn->StopAnimMontage(Animation);
	}
}

void APSE_LYFE_BaseFiringWeapon::SpawnImpactEffects_Implementation(const FHitResult& Impact)
{
	if (ImpactTemplate && Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;

		// trace again to find component lost during replication
		if (!Impact.Component.IsValid())
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;
		}

		APSE_LYFE_HitImpact* EffectActor = GetWorld()->SpawnActorDeferred<APSE_LYFE_HitImpact>(ImpactTemplate, Impact.ImpactPoint, Impact.ImpactNormal.Rotation());
		if (EffectActor)
		{
			EffectActor->SurfaceHit = UseImpact;
			UGameplayStatics::FinishSpawningActor(EffectActor, FTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint));
		}
	}
}

void APSE_LYFE_BaseFiringWeapon::PlayMuzzleFlash()
{
	if (MuzzleFX)
	{
		MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, GetMesh3P(), GetFiringStartLoc());
		MuzzlePSC->SetWorldScale3D(FVector(1));
	}
	if (FiringSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FiringSound, GetActorLocation(), 0.7);
	}
}

void APSE_LYFE_BaseFiringWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_BaseFiringWeapon, FireCounter, COND_SkipOwner);
}