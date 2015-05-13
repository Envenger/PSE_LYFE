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
#include "PSE_LYFE_Character4_Weapon.h"

APSE_LYFE_Character4_Weapon::APSE_LYFE_Character4_Weapon()
{
	GrenadeComp = CreateDefaultSubobject<UPSE_LYFE_GrenadeComponent>(TEXT("GrenadeComponent"));
	GrenadeComp->SetIsReplicated(true);
}

void APSE_LYFE_Character4_Weapon::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("LeftClick", IE_Pressed, this, &APSE_LYFE_Character4_Weapon::LeftClickPressed);
	InputComponent->BindAction("LeftClick", IE_Released, this, &APSE_LYFE_Character4_Weapon::LeftClickReleased);

	InputComponent->BindAction("GrenadeThrow", IE_Pressed, this, &APSE_LYFE_Character4_Weapon::GrenadeThrowPressed);
	InputComponent->BindAction("GrenadeThrow", IE_Released, this, &APSE_LYFE_Character4_Weapon::GrenadeThrowReleased);

	InputComponent->BindAction("Reload", IE_Pressed, this, &APSE_LYFE_Character4_Weapon::StartWeaponReload);

	InputComponent->BindAction("Weapon1", IE_Pressed, this, &APSE_LYFE_Character4_Weapon::ChangeWeaponTo1);
	InputComponent->BindAction("Weapon2", IE_Pressed, this, &APSE_LYFE_Character4_Weapon::ChangeWeaponTo2);
}

void APSE_LYFE_Character4_Weapon::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		SpawnDefaultWeapon();
	}
}


void APSE_LYFE_Character4_Weapon::SpawnDefaultWeapon()
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

void APSE_LYFE_Character4_Weapon::StartWeaponFire()
{
	if (CanFire() && GetCurrentWeapon())
	{
		if (GetCurrentWeapon()->CurrentState != EWeaponState::Passive)
		{
			GetCurrentWeapon()->StartFire();
		}
	}
}


void APSE_LYFE_Character4_Weapon::StopWeaponFire()
{
	if (GetCurrentWeapon())
	{
		if (GetCurrentWeapon()->IsA(APSE_LYFE_AutoRWeapon::StaticClass()))
		{
			GetCurrentWeapon()->StopFire();
		}
	}
}


void APSE_LYFE_Character4_Weapon::StartWeaponReload()
{
	APSE_LYFE_BaseWeapon* TempWeapon = GetCurrentWeapon();
	if (TempWeapon->IsA(APSE_LYFE_ReloadableWeapon::StaticClass()))
	{
		APSE_LYFE_ReloadableWeapon* ReloadableWeapon = Cast<APSE_LYFE_ReloadableWeapon>(TempWeapon);
		if (ReloadableWeapon->CanReload() && (ReloadableWeapon->CurrentState == EWeaponState::Firing ||
			ReloadableWeapon->CurrentState == EWeaponState::Idle))
		{
			if (bIsSprinting == true)
			{
				EndSprint();
			}
			ReloadableWeapon->StartReload();
		}
	}
}

bool APSE_LYFE_Character4_Weapon::CanFire()
{
	return true;
}

void APSE_LYFE_Character4_Weapon::ChangeWeaponTo(uint8 NewWeaponIndex)
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

void APSE_LYFE_Character4_Weapon::CancleWeaponReload()
{
	APSE_LYFE_BaseWeapon* TempWeapon = GetCurrentWeapon();
	if (TempWeapon->IsA(APSE_LYFE_ReloadableWeapon::StaticClass()))
	{
		APSE_LYFE_ReloadableWeapon* ReloadableWeapon = Cast<APSE_LYFE_ReloadableWeapon>(TempWeapon);
		if (ReloadableWeapon->CurrentState == EWeaponState::Reloading)
		{
			ReloadableWeapon->CancelReload();
		}
	}
}

bool APSE_LYFE_Character4_Weapon::ServerChangeWeaponTo_Validate(uint8 NewWeaponIndex)
{
	return true;
}
void APSE_LYFE_Character4_Weapon::ServerChangeWeaponTo_Implementation(uint8 NewWeaponIndex)
{
	if (CurrentWeaponIndex != 0 && GetCurrentWeapon()->CurrentState == EWeaponState::Idle)
	{
		float UnEquipTime = GetCurrentWeapon()->UnEquippingTime;
		GetCurrentWeapon()->StartUnEquip();
		FTimerHandle UniqueHandle;
		FTimerDelegate WeaponChangeDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_Character4_Weapon::EquipWeapon, NewWeaponIndex);
		GetWorldTimerManager().SetTimer(UniqueHandle, WeaponChangeDelegate, UnEquipTime, false);
	}
}

void APSE_LYFE_Character4_Weapon::EquipWeapon(uint8 NewWeaponIndex)
{
	CurrentWeaponIndex = NewWeaponIndex;
	GetCurrentWeapon()->StartEquip();
}


void APSE_LYFE_Character4_Weapon::ChangeWeaponTo1()
{
	ChangeWeaponTo(1);
}

void APSE_LYFE_Character4_Weapon::ChangeWeaponTo2()
{
	ChangeWeaponTo(2);
}

void APSE_LYFE_Character4_Weapon::LeftClickPressed()
{
	if (bIsSprinting == true)
	{
		EndSprint();
	}
	bWantsToFire = true;
	if (GrenadeComp->CurrentGrenadeState == EGrenadeState::Null)
	{
		StartWeaponFire();
	}
}

void APSE_LYFE_Character4_Weapon::LeftClickReleased()
{
	bWantsToFire = false;
	StopWeaponFire();
}


APSE_LYFE_BaseWeapon* APSE_LYFE_Character4_Weapon::GetCurrentWeapon()
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

APSE_LYFE_BaseWeapon* APSE_LYFE_Character4_Weapon::GetWeaponWithIndex(uint8 Index) const
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

void APSE_LYFE_Character4_Weapon::GrenadeThrowPressed()
{
	if (GrenadeComp)
	{
		GrenadeComp->ThrowGrenadeStart();
	}
}

void APSE_LYFE_Character4_Weapon::GrenadeThrowReleased()
{
	if (GrenadeComp)
	{
		GrenadeComp->ThrowGrenadeFinish();
	}
}

const TArray<APSE_LYFE_BaseWeapon*> APSE_LYFE_Character4_Weapon::GetAllWeapons() const
{
	TArray<APSE_LYFE_BaseWeapon*> Weapons;
	for (int8 i = 1; i <= 3; i++)
	{
		APSE_LYFE_BaseWeapon* TempWeapon = GetWeaponWithIndex(i);
		if (TempWeapon)
		{
			Weapons.Add(TempWeapon);
		}
	}
	return Weapons;
}

void APSE_LYFE_Character4_Weapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character4_Weapon, CurrentWeaponIndex, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character4_Weapon, CurrentArmedWeapons, COND_OwnerOnly);
}

