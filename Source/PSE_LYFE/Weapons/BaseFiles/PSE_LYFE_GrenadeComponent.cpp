// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "PSE_LYFE_BaseGrenade.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_GrenadeComponent.h"

UPSE_LYFE_GrenadeComponent::UPSE_LYFE_GrenadeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoActivate = true;
	// Grenade
	ThrowStartTimer = 0.767;
	ThrowEndTimer = 0.206;
	CurrentGrenadeState = EGrenadeState::Null;
}

void UPSE_LYFE_GrenadeComponent::OnRep_GrenadeAnimation()
{
	APSE_LYFE_Character4_Weapon* Character = Cast<APSE_LYFE_Character4_Weapon>(GetOwner());
	
	if (CurrentGrenadeState == EGrenadeState::ThrowStart)
	{
		Character->PlayAnimMontage(GrenadeMontage, 1, "GrenadeThrowStart");
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "start");
	}
	else if (CurrentGrenadeState == EGrenadeState::ThrowEnd)
	{
		//GrenadeEndAnimation->section
		Character->PlayAnimMontage(GrenadeMontage, 1, "GrenadeThrowEnd");
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, "End");
	}
}



/////////////////////////////////////////////////////////////
// Grenade


void UPSE_LYFE_GrenadeComponent::ThrowGrenadeStart()
{
	if (GetOwner()->IsA(APSE_LYFE_Character4_Weapon::StaticClass()))
	{
		APSE_LYFE_Character4_Weapon* Character = Cast<APSE_LYFE_Character4_Weapon>(GetOwner());
		if (Character->bWantsToFire == true)
		{
			Character->StopWeaponFire();
		}
		ServerThrowGrenadeStart();
	}
}


bool UPSE_LYFE_GrenadeComponent::ServerThrowGrenadeStart_Validate()
{
	return true;
}

void UPSE_LYFE_GrenadeComponent::ServerThrowGrenadeStart_Implementation()
{
	if (CurrentGrenadeState == EGrenadeState::Null)
	{
		APSE_LYFE_Character4_Weapon* Character = Cast<APSE_LYFE_Character4_Weapon>(GetOwner());
		Character->PlayAnimMontage(GrenadeMontage, 1, "GrenadeThrowStart");
		CurrentGrenadeState = EGrenadeState::ThrowStart;
		Character->GetWorldTimerManager().SetTimer(GrenadeLoopTimerHandle, this, &UPSE_LYFE_GrenadeComponent::GrenadeLoopStart, ThrowStartTimer, false);
	}
}

void UPSE_LYFE_GrenadeComponent::GrenadeLoopStart()
{
	if (CurrentGrenadeState == EGrenadeState::ThrowStart)
	{
		CurrentGrenadeState = EGrenadeState::ThrowLoop;
	}
}

void UPSE_LYFE_GrenadeComponent::ThrowGrenadeFinish()
{
	if (GetOwner()->IsA(APSE_LYFE_Character4_Weapon::StaticClass()))
	{
		ServerThrowGrenadeFinish();
	}
}

bool UPSE_LYFE_GrenadeComponent::ServerThrowGrenadeFinish_Validate()
{
	return true;
}

//_Implementation
void UPSE_LYFE_GrenadeComponent::ServerThrowGrenadeFinish_Implementation()
{
	APSE_LYFE_Character4_Weapon* Character = Cast<APSE_LYFE_Character4_Weapon>(GetOwner());
	FTimerHandle GrenadeThrowHandle;
	if (CurrentGrenadeState == EGrenadeState::ThrowStart)
	{
		float TimeRemaining = Character->GetWorldTimerManager().GetTimerElapsed(GrenadeLoopTimerHandle);
		Character->GetWorldTimerManager().ClearTimer(GrenadeLoopTimerHandle);

		float TimerComplitionRatio = TimeRemaining / ThrowStartTimer;
		
		FTimerDelegate GrenadeSpawnDelegate = FTimerDelegate::CreateUObject(this, &UPSE_LYFE_GrenadeComponent::SpawnGrenade, TimerComplitionRatio);
		Character->GetWorldTimerManager().SetTimer(GrenadeThrowHandle, GrenadeSpawnDelegate, 0.1, false);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, FString::SanitizeFloat(TimerComplitionRatio));
		Character->PlayAnimMontage(GrenadeMontage, 1, "GrenadeThrowEnd");
		CurrentGrenadeState = EGrenadeState::ThrowEnd;
	}
	else if (CurrentGrenadeState == EGrenadeState::ThrowLoop)
	{
		FTimerDelegate GrenadeSpawnDelegate = FTimerDelegate::CreateUObject(this, &UPSE_LYFE_GrenadeComponent::SpawnGrenade, 1.0f);
		Character->GetWorldTimerManager().SetTimer(GrenadeThrowHandle, GrenadeSpawnDelegate, 0.05, false);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, "Full");
		Character->PlayAnimMontage(GrenadeMontage, 1, "GrenadeThrowEnd");
		CurrentGrenadeState = EGrenadeState::ThrowEnd;
	}
}

void UPSE_LYFE_GrenadeComponent::SpawnGrenade(float TimerComplitionRatio)
{
	APSE_LYFE_Character4_Weapon* Character = Cast<APSE_LYFE_Character4_Weapon>(GetOwner());
	FVector  SpawnLocation = Character->GetMesh()->GetSocketLocation("GrenadeSpawn");
	FRotator SpawnRotation = FRotator(0, Character->GetBaseAimRotation().Yaw, 0);
	APSE_LYFE_BaseGrenade* Grenade = GetWorld()->SpawnActor<APSE_LYFE_BaseGrenade>(GrenadeClass, SpawnLocation, SpawnRotation);
	Grenade->ProjectileComp->Velocity *= FMath::Max(0.3f, TimerComplitionRatio);
	CurrentGrenadeState = EGrenadeState::Null;
}

void UPSE_LYFE_GrenadeComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPSE_LYFE_GrenadeComponent, CurrentGrenadeState);
}