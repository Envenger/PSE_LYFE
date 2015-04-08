// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/PSE_LYFE_ArmedCharacter.h"
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

	MaxForwardThrowVelocity = 550;
	MaxUpThrowVelocity = 400;

	MinForwardThrowVelocity = 300;
	MinUpThrowVelocity = 120;
}

void UPSE_LYFE_GrenadeComponent::OnRep_GrenadeAnimation()
{
	APSE_LYFE_ArmedCharacter* Character = Cast<APSE_LYFE_ArmedCharacter>(GetOwner());
	
	if (CurrentGrenadeState == EGrenadeState::ThrowStart)
	{
		Character->PlayAnimMontage(GrenadeStartAnimation);
	}
	else if (CurrentGrenadeState == EGrenadeState::ThrowLoop)
	{
		Character->PlayAnimMontage(GrenadeLoopAnimation);
	}
	else if (CurrentGrenadeState == EGrenadeState::ThrowEnd)
	{
		Character->PlayAnimMontage(GrenadeEndAnimation);
	}
}



/////////////////////////////////////////////////////////////
// Grenade


void UPSE_LYFE_GrenadeComponent::ClientThrowGrenadeStart()
{
	if (GetOwner()->IsA(APSE_LYFE_ArmedCharacter::StaticClass()))
	{
		APSE_LYFE_ArmedCharacter* Character = Cast<APSE_LYFE_ArmedCharacter>(GetOwner());
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
		APSE_LYFE_ArmedCharacter* Character = Cast<APSE_LYFE_ArmedCharacter>(GetOwner());
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

void UPSE_LYFE_GrenadeComponent::ClientThrowGrenadeFinish()
{
	if (GetOwner()->IsA(APSE_LYFE_ArmedCharacter::StaticClass()))
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
	APSE_LYFE_ArmedCharacter* Character = Cast<APSE_LYFE_ArmedCharacter>(GetOwner());
	FTimerHandle UniqueHandle;
	if (CurrentGrenadeState == EGrenadeState::ThrowStart)
	{
		float TimeRemaining = Character->GetWorldTimerManager().GetTimerElapsed(GrenadeLoopTimerHandle);
		Character->GetWorldTimerManager().ClearTimer(GrenadeLoopTimerHandle);

		float TimerComplitionRatio = TimeRemaining / ThrowStartTimer;
		float UpForce = FMath::Lerp(MinUpThrowVelocity, MaxUpThrowVelocity, TimerComplitionRatio);
		float ForwardForce = FMath::Lerp(MinForwardThrowVelocity, MaxForwardThrowVelocity, TimerComplitionRatio);
		FTimerDelegate GrenadeSpawnDelegate = FTimerDelegate::CreateUObject(this, &UPSE_LYFE_GrenadeComponent::SpawnGrenade, ForwardForce, UpForce);
		Character->GetWorldTimerManager().SetTimer(UniqueHandle, GrenadeSpawnDelegate, ThrowEndTimer, false);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, FString::SanitizeFloat(TimerComplitionRatio));
		CurrentGrenadeState = EGrenadeState::ThrowEnd;
	}
	else if (CurrentGrenadeState == EGrenadeState::ThrowLoop)
	{
		FTimerDelegate GrenadeSpawnDelegate = FTimerDelegate::CreateUObject(this, &UPSE_LYFE_GrenadeComponent::SpawnGrenade, MaxForwardThrowVelocity, MaxUpThrowVelocity);
		Character->GetWorldTimerManager().SetTimer(UniqueHandle, GrenadeSpawnDelegate, ThrowEndTimer, false);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Cyan, "Full");
		CurrentGrenadeState = EGrenadeState::ThrowEnd;
	}
}

void UPSE_LYFE_GrenadeComponent::SpawnGrenade(float ForwardForce, float VerticalForce)
{
	APSE_LYFE_ArmedCharacter* Character = Cast<APSE_LYFE_ArmedCharacter>(GetOwner());
	
	float Direction = Character->ViewDirection.Rotation().Yaw;
	FQuat ForceQuat = FRotator(0, Direction, 0).Quaternion();
	FVector Force(ForwardForce, 0, VerticalForce);
	Force = ForceQuat.RotateVector(Force);
	Force *= 30;
	FVector  SpawnLocation = Character->GetMesh()->GetSocketLocation("GrenadeLoc");
	SpawnLocation += ForceQuat.RotateVector(FVector(45, 0, 15));
	APSE_LYFE_BaseGrenade* Grenade = GetWorld()->SpawnActor<APSE_LYFE_BaseGrenade>(GrenadeClass, SpawnLocation, FRotator(0, 0, 0));
	Grenade->GrenadeMesh->AddImpulse(Force);
	CurrentGrenadeState = EGrenadeState::Null;
}

void UPSE_LYFE_GrenadeComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPSE_LYFE_GrenadeComponent, CurrentGrenadeState);
}