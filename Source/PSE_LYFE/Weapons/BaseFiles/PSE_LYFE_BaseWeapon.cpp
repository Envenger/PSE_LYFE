// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "UnrealNetwork.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "Weapons/Effects/PSE_LYFE_HitImpact.h"
#include "PSE_LYFE_BaseWeapon.h"

APSE_LYFE_BaseWeapon::APSE_LYFE_BaseWeapon(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Mesh3P = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("WeaponMesh3P"));
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Mesh3P;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	FiringRate = 0.1;
	EquippingTime = 0.9;
	UnEquippingTime = 0.9;

	bIsWeaponEquiped = false;

	CurrentState = EWeaponState::Null;
	ReloadingType = EReloadingType::Null;
	WeaponCategory = EWeaponCategory::Primary;

	bCanRecoil = true;
}

void APSE_LYFE_BaseWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	FireCounter = 0;
}

void APSE_LYFE_BaseWeapon::Tick(float DeltaSeconds)
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

void APSE_LYFE_BaseWeapon::SetOwningPawn(APSE_LYFE_Character4_Weapon* NewOwner)
{
	MyPawn = NewOwner;
	Instigator = NewOwner;
	SetOwner(NewOwner);
	CurrentState = EWeaponState::Passive;
	AttachMeshToPawn();
	SetActorHiddenInGame(true);
}

void APSE_LYFE_BaseWeapon::StartEquip()
{
	SetActorHiddenInGame(false);
	CurrentState = EWeaponState::Passive;
	ClientStartEquip();
	FTimerHandle EquipTimerHandle;
	GetWorldTimerManager().SetTimer(EquipTimerHandle, this, &APSE_LYFE_BaseWeapon::FinishEquip, EquippingTime, false);
}

void APSE_LYFE_BaseWeapon::ClientStartEquip_Implementation()
{
	// EquipingAnimation
	CurrentState = EWeaponState::Passive;
}

void APSE_LYFE_BaseWeapon::FinishEquip()
{
	CurrentState = EWeaponState::Idle;
	bIsWeaponEquiped = true;

}

void APSE_LYFE_BaseWeapon::StartUnEquip()
{
	CurrentState = EWeaponState::Passive;
	ClientStartUnEquip();
	FTimerHandle UnEquipTimerHandle;
	GetWorldTimerManager().SetTimer(UnEquipTimerHandle, this, &APSE_LYFE_BaseWeapon::FinishUnEquip, UnEquippingTime, false);
}

void APSE_LYFE_BaseWeapon::ClientStartUnEquip_Implementation()
{
	CurrentState = EWeaponState::Passive;
	// Un-equpiing animation
}

void APSE_LYFE_BaseWeapon::FinishUnEquip()
{
	SetActorHiddenInGame(true);
	bIsWeaponEquiped = false;
}

void APSE_LYFE_BaseWeapon::OnRep_WeaponEquipReplication()
{
	if (bIsWeaponEquiped == true)
	{
		SetActorHiddenInGame(false);
		CurrentState = EWeaponState::Idle;
		if (MyPawn->bWantsToFire == true)
		{
			StartFire();
		}
	}
	else if (bIsWeaponEquiped == false)
	{
		SetActorHiddenInGame(true);
		CurrentState = EWeaponState::Passive;
	}
}

void APSE_LYFE_BaseWeapon::TestState(FColor Colour)
{
	if (CurrentState == EWeaponState::Idle)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, Colour, "Idle");
	}
	else if (CurrentState == EWeaponState::Null)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, Colour, "Null");
	}
	else if (CurrentState == EWeaponState::ReadyToFire)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, Colour, "Ready to fire");
	}
	else if (CurrentState == EWeaponState::Firing)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, Colour, "Firing");
	}
	else if (CurrentState == EWeaponState::Equipping)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, Colour, "Equipping");
	}
	else if (CurrentState == EWeaponState::Reloading)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, Colour, "Reloading");
	}
}

void APSE_LYFE_BaseWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		FName AttachPoint = FName(TEXT("WeaponSocket_Rifle"));
		USkeletalMeshComponent* UsePawnMesh = MyPawn->GetMesh();
		AttachRootComponentTo(UsePawnMesh, AttachPoint, EAttachLocation::KeepRelativeOffset, false);
	}
}

void APSE_LYFE_BaseWeapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_BaseWeapon, FireCounter, COND_SkipOwner);
	DOREPLIFETIME(APSE_LYFE_BaseWeapon, MyPawn);
	DOREPLIFETIME(APSE_LYFE_BaseWeapon, bIsWeaponEquiped);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APSE_LYFE_BaseWeapon::StartFire()
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

bool APSE_LYFE_BaseWeapon::ServerStartFire_Validate()
{
	return true;
}

void APSE_LYFE_BaseWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

void APSE_LYFE_BaseWeapon::Fire()
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

bool APSE_LYFE_BaseWeapon::PreFireChecks()
{
	return true;
}

void APSE_LYFE_BaseWeapon::StopFire()
{

}

void APSE_LYFE_BaseWeapon::ClientFire()
{
}


bool APSE_LYFE_BaseWeapon::ServerStopFire_Validate()
{
	return true;
}

void APSE_LYFE_BaseWeapon::ServerStopFire_Implementation()
{
	StopFire();
}


FHitResult APSE_LYFE_BaseWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace) const
{
	static FName WeaponFireTag = FName(TEXT("WeaponTrace"));

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, true, Instigator);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, ECC_PhysicsBody, TraceParams);

	return Hit;
}

FVector APSE_LYFE_BaseWeapon::CalcRecoilDirection(FVector OriginalDirection)
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

void APSE_LYFE_BaseWeapon::DoRecoil()
{
	if (Role == ROLE_Authority)
	{
		float StaminaDec = FMath::Min(MyPawn->CurrentStamina, Recoil.StaminaDrain);

		MyPawn->CurrentStamina -= StaminaDec;
	}
	Recoil.DoRecoil();
}

FName APSE_LYFE_BaseWeapon::GetFiringStartLoc()
{
	return FName(TEXT("FiringPoint"));
}

void APSE_LYFE_BaseWeapon::OnRep_UpdateClientFireCounter()
{
	if (FireCounter > 0)
	{
		PlayWeaponAnimation(FireAnimation);
	}
	else
	{
		StopWeaponAnimation(FireAnimation);
	}
}

float APSE_LYFE_BaseWeapon::PlayWeaponAnimation(UAnimMontage* Animation, float PlayRate)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		Duration = MyPawn->PlayAnimMontage(Animation, PlayRate);
	}
	return Duration;
}

void APSE_LYFE_BaseWeapon::StopWeaponAnimation(UAnimMontage* Animation)
{
	if (MyPawn)
	{
		MyPawn->StopAnimMontage(Animation);
	}
}

void APSE_LYFE_BaseWeapon::SpawnImpactEffects_Implementation(const FHitResult& Impact)
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


