// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Weapons/BaseFiles/PSE_LYFE_BaseWeapon.h"
#include "Weapons/BaseFiles/PSE_LYFE_ReloadableWeapon.h"
#include "Weapons/BaseFiles/PSE_LYFE_AutoRWeapon.h"
#include "Weapons/BaseFiles/PSE_LYFE_SemiAutoRWeapon.h"
#include "Weapons/BaseFiles/PSE_LYFE_GrenadeComponent.h"
#include "Weapons/Zoomable/PSE_LYFE_AutoRZWeapon.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_ArmedCharacter.h"


APSE_LYFE_ArmedCharacter::APSE_LYFE_ArmedCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	bWantsToFire = false;
	CurrentWeaponIndex = 0;

	StaminaRegenRate = 18;

	MaxStamina = 100;
	CurrentStamina = MaxStamina;
	CurrentStaminaRep = MaxStamina;

	GrenadeComp = ObjectInitializer.CreateDefaultSubobject<UPSE_LYFE_GrenadeComponent>(this, TEXT("GrenadeComponent"));
	GrenadeComp->SetIsReplicated(true);
}

void APSE_LYFE_ArmedCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("GrenadeThrow", IE_Pressed, this, &APSE_LYFE_ArmedCharacter::GrenadeThrowPressed);
	InputComponent->BindAction("GrenadeThrow", IE_Released, this, &APSE_LYFE_ArmedCharacter::GrenadeThrowReleased);

	InputComponent->BindAction("Reload", IE_Pressed, this, &APSE_LYFE_ArmedCharacter::StartWeaponReload);

	InputComponent->BindAction("Weapon1", IE_Pressed, this, &APSE_LYFE_ArmedCharacter::ChangeWeaponTo1);
	InputComponent->BindAction("Weapon2", IE_Pressed, this, &APSE_LYFE_ArmedCharacter::ChangeWeaponTo2);
}


void APSE_LYFE_ArmedCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		SpawnDefaultWeapon();
	}
}

void APSE_LYFE_ArmedCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentStamina < MaxStamina)
	{
		float StaminaIncrease = FMath::Min((MaxStamina - CurrentStamina), (StaminaRegenRate * DeltaSeconds));
		CurrentStamina += StaminaIncrease;
		CurrentStaminaRep = FMath::RoundToInt(CurrentStamina);
	}
}

void APSE_LYFE_ArmedCharacter::SpawnDefaultWeapon()
{
	if (Role < ROLE_Authority)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (DefaultInventory.PrimaryWeapon)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APSE_LYFE_BaseWeapon* TempWeapon = GetWorld()->SpawnActor<APSE_LYFE_BaseWeapon>(DefaultInventory.PrimaryWeapon);
			TempWeapon->SetOwningPawn(this);
			CurrentArmedWeapons.PrimaryWeapon = TempWeapon;
			if (CurrentWeaponIndex == 0)
			{
				TempWeapon->StartEquip();
				CurrentWeaponIndex = 1;
			}
		}
	}
	if (DefaultInventory.SecondaryWeapon)
	{
		if (World)
		{
			APSE_LYFE_BaseWeapon* TempWeapon = GetWorld()->SpawnActor<APSE_LYFE_BaseWeapon>(DefaultInventory.SecondaryWeapon);
			TempWeapon->SetOwningPawn(this);
			CurrentArmedWeapons.SecondaryWeapon = TempWeapon;
			if (CurrentWeaponIndex == 0)
			{
				TempWeapon->StartEquip();
				CurrentWeaponIndex = 2;
			}
		}
	}
}

void APSE_LYFE_ArmedCharacter::StartWeaponFire()
{
	if (CanFire() && GetCurrentWeapon())
	{
		if (GetCurrentWeapon()->CurrentState != EWeaponState::Passive)
		{
			GetCurrentWeapon()->StartFire();
		}
	}
}


void APSE_LYFE_ArmedCharacter::StopWeaponFire()
{
	if (GetCurrentWeapon())
	{
		if (GetCurrentWeapon()->IsA(APSE_LYFE_AutoRWeapon::StaticClass()))
		{
			GetCurrentWeapon()->StopFire();
		}
	}
}


void APSE_LYFE_ArmedCharacter::StartWeaponReload()
{
	APSE_LYFE_BaseWeapon* TempWeapon = GetCurrentWeapon();
	if (TempWeapon->IsA(APSE_LYFE_ReloadableWeapon::StaticClass()))
	{
		APSE_LYFE_ReloadableWeapon* ReloadableWeapon = Cast<APSE_LYFE_ReloadableWeapon>(TempWeapon);
		if (ReloadableWeapon->CanReload() && (ReloadableWeapon->CurrentState == EWeaponState::Firing ||
		ReloadableWeapon->CurrentState == EWeaponState::Idle))
		{
			ReloadableWeapon->StartReload();
		}
	}
}

bool APSE_LYFE_ArmedCharacter::CanFire()
{
	return true;
}

void APSE_LYFE_ArmedCharacter::ChangeWeaponTo(uint8 NewWeaponIndex)
{
	if (CurrentWeaponIndex != NewWeaponIndex && (GetWeaponWithIndex(NewWeaponIndex) != nullptr) && CharacterHUD->bIsInventoryOpen == false)
	{
		if (CurrentWeaponIndex != 0)
		{
			if (GetCurrentWeapon()->IsA(APSE_LYFE_AutoRWeapon::StaticClass()))
			{
				APSE_LYFE_AutoRWeapon* TempWeapon = Cast<APSE_LYFE_AutoRWeapon>(GetCurrentWeapon());
				if (TempWeapon->CurrentState == EWeaponState::Firing)
				{
					TempWeapon->StopFire();
				}
				else if (TempWeapon->CurrentState == EWeaponState::Reloading)
				{
					TempWeapon->CancelReload();
				}
			}
			else if (GetCurrentWeapon()->IsA(APSE_LYFE_SemiAutoRWeapon::StaticClass()))
			{
				APSE_LYFE_SemiAutoRWeapon* TempWeapon = Cast<APSE_LYFE_SemiAutoRWeapon>(GetCurrentWeapon());
				if (TempWeapon->CurrentState == EWeaponState::Firing)
				{
					TempWeapon->StopFire();
				}
				else if (TempWeapon->CurrentState == EWeaponState::Reloading)
				{
					TempWeapon->CancelReload();
				}
			}
		}
		ServerChangeWeaponTo(NewWeaponIndex);
	}
}

bool APSE_LYFE_ArmedCharacter::ServerChangeWeaponTo_Validate(uint8 NewWeaponIndex)
{
	return true;
}
void APSE_LYFE_ArmedCharacter::ServerChangeWeaponTo_Implementation(uint8 NewWeaponIndex)
{
	if (CurrentWeaponIndex != 0 && GetCurrentWeapon()->CurrentState == EWeaponState::Idle)
	{
		float UnEquipTime = GetCurrentWeapon()->UnEquippingTime;
		GetCurrentWeapon()->StartUnEquip();
		FTimerHandle UniqueHandle;
		FTimerDelegate WeaponChangeDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_ArmedCharacter::EquipWeapon, NewWeaponIndex);
		GetWorldTimerManager().SetTimer(UniqueHandle, WeaponChangeDelegate, UnEquipTime, false);
	}
}

void APSE_LYFE_ArmedCharacter::EquipWeapon(uint8 NewWeaponIndex)
{
	CurrentWeaponIndex = NewWeaponIndex;
	GetCurrentWeapon()->StartEquip();
}


void APSE_LYFE_ArmedCharacter::ChangeWeaponTo1()
{
	ChangeWeaponTo(1);
}

void APSE_LYFE_ArmedCharacter::ChangeWeaponTo2()
{
	ChangeWeaponTo(2);
}

void APSE_LYFE_ArmedCharacter::LeftClickPressed()
{
	Super::LeftClickPressed();
	bWantsToFire = true;
	if (GrenadeComp->CurrentGrenadeState == EGrenadeState::Null)
	{
		StartWeaponFire();
	}
}

void APSE_LYFE_ArmedCharacter::LeftClickReleased()
{
	Super::LeftClickReleased();
	bWantsToFire = false;
	StopWeaponFire();
}


APSE_LYFE_BaseWeapon* APSE_LYFE_ArmedCharacter::GetCurrentWeapon()
{
	if (CurrentWeaponIndex == 1)
	{
		return CurrentArmedWeapons.PrimaryWeapon;
	}
	else if (CurrentWeaponIndex == 2)
	{
		return CurrentArmedWeapons.SecondaryWeapon;
	}
	else if (CurrentWeaponIndex == 3)
	{
		return CurrentArmedWeapons.MeleeWeapon;
	}
	else
	{
		return nullptr;
	}
}

APSE_LYFE_BaseWeapon* APSE_LYFE_ArmedCharacter::GetWeaponWithIndex(uint8 Index)
{
	if (Index == 1)
	{
		return CurrentArmedWeapons.PrimaryWeapon;
	}
	else if (Index == 2)
	{
		return CurrentArmedWeapons.SecondaryWeapon;
	}
	else if (Index == 3)
	{
		return CurrentArmedWeapons.MeleeWeapon;
	}
	else
	{
		return nullptr;
	}
}

void APSE_LYFE_ArmedCharacter::GrenadeThrowPressed()
{
	if (GrenadeComp)
	{
		GrenadeComp->ClientThrowGrenadeStart();
	}
}

void APSE_LYFE_ArmedCharacter::GrenadeThrowReleased()
{
	if (GrenadeComp)
	{
		GrenadeComp->ClientThrowGrenadeFinish();
	}
}


void APSE_LYFE_ArmedCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_ArmedCharacter, CurrentWeaponIndex, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_ArmedCharacter, CurrentArmedWeapons, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_ArmedCharacter, CurrentStaminaRep, COND_OwnerOnly);

}


