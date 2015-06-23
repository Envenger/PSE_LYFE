// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Weapons/BaseFiles/PSE_LYFE_BaseWeapon.h"
#include "Weapons/BaseFiles/PSE_LYFE_GrenadeComponent.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "Items/PSE_LYFE_BaseInventoryItem.h"
#include "Items/Weapon/PSE_LYFE_BaseWeaponItem.h"
#include "UnrealNetwork.h"
#include "Player/PSE_LYFE_AnimInstance.h"
#include "PSE_LYFE_Character4_Weapon.h"

APSE_LYFE_Character4_Weapon::APSE_LYFE_Character4_Weapon()
{
	GrenadeComp = CreateDefaultSubobject<UPSE_LYFE_GrenadeComponent>(TEXT("GrenadeComponent"));
	GrenadeComp->SetIsReplicated(true);

	AnimBP_bIKLeftHand = true;

	CurrentWeaponIndex = -1; // -1 Index means no weapon is equipped

	CurrentArmedWeapons.Add(nullptr);
	CurrentArmedWeapons.Add(nullptr);
	CurrentArmedWeapons.Add(nullptr);

	ClientCurrentArmedWeapons.Add(nullptr);
	ClientCurrentArmedWeapons.Add(nullptr);
	ClientCurrentArmedWeapons.Add(nullptr);

	bIsWeaponChanging = false;

	AnimBP_EquipSlot = -1;
	AnimBP_UnEquipSlot = -1;
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

void APSE_LYFE_Character4_Weapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(NULL) || !GetCurrentWeapon())
	{
		AnimBP_bIKLeftHand = false;
	}
	else
	{
		AnimBP_bIKLeftHand = true;
	}

	if (AnimBP_bIKLeftHand)
	{
		CoverAnimInstance->HandAttachmentLocation = LeftHandWeaponAttachmentLocation();
	}
}

void APSE_LYFE_Character4_Weapon::SpawnDefaultWeapon()
{
	if (Role == ROLE_Authority)
	{
		TArray<TSubclassOf<APSE_LYFE_BaseWeapon>> DefaultWeaponsArray;
		DefaultWeaponsArray.Add(DefaultInventory.PrimaryWeapon);
		DefaultWeaponsArray.Add(DefaultInventory.SecondaryWeapon);
		DefaultWeaponsArray.Add(DefaultInventory.MeleeWeapon);

		for (int8 i = 0; i <= 2; i++)
		{
			if (DefaultWeaponsArray[i])
			{
				SpawnWeapon(DefaultWeaponsArray[i], i);
			}
		}
	}
}

bool APSE_LYFE_Character4_Weapon::SpawnWeapon(TSubclassOf<APSE_LYFE_BaseWeapon> WeaponClass, int8 WeaponSlotIndex)
{
	if (WeaponSlotIndex >= 0 && WeaponSlotIndex <= 2)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APSE_LYFE_BaseWeapon* TempWeapon = GetWorld()->SpawnActor<APSE_LYFE_BaseWeapon>(WeaponClass);
			TempWeapon->SetOwningPawn(this);
			const float EquipingTime = TempWeapon->EquipAnimation->SequenceLength;

			if (CurrentWeaponIndex == -1)
			{
				CurrentArmedWeapons[WeaponSlotIndex] = TempWeapon;
				bIsWeaponChanging = true;
				Client_EquipWeapon(WeaponSlotIndex);
				FTimerHandle WeaponChangeHandle;
				FTimerDelegate WeaponChangeDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_Character4_Weapon::SetCurrentWeapon, WeaponSlotIndex);
				GetWorldTimerManager().SetTimer(WeaponChangeHandle, WeaponChangeDelegate, EquipingTime, false);
			}
			else if (CurrentWeaponIndex == WeaponSlotIndex)
			{
				TemporaryWeapon = TempWeapon;
				bIsWeaponChanging = true;
				Client_UnEquipWeapon(WeaponSlotIndex, WeaponSlotIndex);
				const float UnEquipingTime = GetCurrentWeapon()->UnEquipAnimation->SequenceLength;
				FTimerHandle ReplaceRemoveWeaponHandle;
				FTimerDelegate ReplaceRemoveWeaponDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_Character4_Weapon::ReplaceRemoveCurrentWeapon);
				GetWorldTimerManager().SetTimer(ReplaceRemoveWeaponHandle, ReplaceRemoveWeaponDelegate, (UnEquipingTime + EquipingTime), false);
			}
			else
			{
				CurrentArmedWeapons[WeaponSlotIndex] = TempWeapon;
			}
			return true;
		}
	}
	return false;// SLot already has a weapon
}

void APSE_LYFE_Character4_Weapon::SetCurrentWeapon(int8 NewWeaponIndex)
{
	bIsWeaponChanging = false;
	CurrentWeaponIndex = NewWeaponIndex;
}

void APSE_LYFE_Character4_Weapon::ReplaceRemoveCurrentWeapon()
{
	CurrentArmedWeapons[CurrentWeaponIndex]->Destroy();
	CurrentArmedWeapons[CurrentWeaponIndex] = TemporaryWeapon;

	bIsWeaponChanging = false;
}

void APSE_LYFE_Character4_Weapon::OnRep_CurrentWeaponChange()// Just used for animation 
{
	for (int8 i = 0; i <= 2; i++)
	{
		if (GetWeaponWithIndex(i))
		{
			if (i == CurrentWeaponIndex && GetWeaponWithIndex(i))
			{
				GetWeaponWithIndex(i)->Mesh3P->SetVisibility(true);
			}
			else
			{
				GetWeaponWithIndex(i)->Mesh3P->SetVisibility(false);
			}
		}
	}
}

void APSE_LYFE_Character4_Weapon::OnRep_ArmedWeaponsChange()
{
	for (int8 i = 0; i < CurrentArmedWeapons.Num(); i++)
	{
		if (i == CurrentWeaponIndex && CurrentArmedWeapons[i])
		{
			CurrentArmedWeapons[i]->Mesh3P->SetVisibility(true);
		}
		if (ClientCurrentArmedWeapons[i] != CurrentArmedWeapons[i])
		{
			ClientCurrentArmedWeapons[i] = CurrentArmedWeapons[i];
		}
	}
}

void APSE_LYFE_Character4_Weapon::Client_UnEquipWeapon_Implementation(int8 WeaponIndex, int8 NewWeaponIndex)
{
	if(Role != ROLE_Authority)
	{
		StartUnEquipAnimation(WeaponIndex, NewWeaponIndex);
	}
}

void APSE_LYFE_Character4_Weapon::Client_EquipWeapon_Implementation(int8 WeaponIndex)
{
	if (Role != ROLE_Authority)
	{
		StartEquipAnimation(WeaponIndex);
	}
}

void APSE_LYFE_Character4_Weapon::StartUnEquipAnimation(int8 WeaponIndex, int8 NewWeaponIndex)
{
	PlayAnimMontage(CurrentArmedWeapons[WeaponIndex]->UnEquipAnimation);
	bIsWeaponChanging = true;
	float UnEquipTime = CurrentArmedWeapons[WeaponIndex]->UnEquipAnimation->CalculateSequenceLength();
	FTimerHandle FinishUnEquipHandle;
	FTimerDelegate FinishUnEquipDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_Character4_Weapon::FinishUnEquipAnimation, WeaponIndex, NewWeaponIndex);
	GetWorldTimerManager().SetTimer(FinishUnEquipHandle, FinishUnEquipDelegate, UnEquipTime, false);
	AnimBP_UnEquipSlot = WeaponIndex;
}

void APSE_LYFE_Character4_Weapon::AnimBP_UnEquipNotify()
{
	if (CurrentArmedWeapons[AnimBP_UnEquipSlot])
	{
		CurrentArmedWeapons[AnimBP_UnEquipSlot]->Mesh3P->SetVisibility(false);
	}
}

void APSE_LYFE_Character4_Weapon::FinishUnEquipAnimation(int8 WeaponIndex, int8 NewWeaponIndex)
{
	if (NewWeaponIndex != -1)
	{
		StartEquipAnimation(NewWeaponIndex);
	}
	else
	{
		CurrentWeaponIndex = -1;// Setting in client side cause the player doesn't have weapons any more
		bIsWeaponChanging = false;
	}
}

void APSE_LYFE_Character4_Weapon::StartEquipAnimation(int8 WeaponIndex)
{
	if (CurrentArmedWeapons[WeaponIndex])
	{
		bIsWeaponChanging = true;
		PlayAnimMontage(CurrentArmedWeapons[WeaponIndex]->EquipAnimation);

		const float EquipingTime = CurrentArmedWeapons[WeaponIndex]->EquipAnimation->SequenceLength;
		FTimerHandle FinishEquipHandle;
		GetWorld()->GetTimerManager().SetTimer(FinishEquipHandle, this, &APSE_LYFE_Character4_Weapon::FinishEquipAnimation, EquipingTime);

		AnimBP_EquipSlot = WeaponIndex;
	}
}

void APSE_LYFE_Character4_Weapon::AnimBP_EquipNotify()
{
	if (AnimBP_EquipSlot == CurrentWeaponIndex)
	{
		CurrentArmedWeapons[AnimBP_EquipSlot] = TemporaryWeapon;
	}
	if (CurrentArmedWeapons[AnimBP_EquipSlot])
	{
		CurrentArmedWeapons[AnimBP_EquipSlot]->Mesh3P->SetVisibility(true);
	}
}

void APSE_LYFE_Character4_Weapon::FinishEquipAnimation()
{
	bIsWeaponChanging = false;
}

void APSE_LYFE_Character4_Weapon::PlayerAddWeapon(TSubclassOf<class APSE_LYFE_BaseWeapon> WeaponClass, const uint8 WeaponSlot)
{
	if (CurrentWeaponIndex == WeaponSlot && GetCurrentWeapon())
	{
		SpawnWeapon(WeaponClass, WeaponSlot);
	}
	else
	{
		SpawnWeapon(WeaponClass, WeaponSlot);
	}
}

void APSE_LYFE_Character4_Weapon::PlayerRemoveWeapon(const int8 WeaponSlot)
{
	if (CurrentArmedWeapons[WeaponSlot] != nullptr)
	{
		if (CurrentWeaponIndex == WeaponSlot)
		{
			int8 NextWeapon = GetNextWeapon(WeaponSlot);
			Client_UnEquipWeapon(WeaponSlot, NextWeapon);
			float UnEquipTime = CurrentArmedWeapons[WeaponSlot]->UnEquipAnimation->CalculateSequenceLength();
			FTimerHandle DeSpawnWeaponHandle;
			FTimerDelegate DeSpawnWeaponDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_Character4_Weapon::DeSpawnWeapon, WeaponSlot, NextWeapon);
			GetWorldTimerManager().SetTimer(DeSpawnWeaponHandle, DeSpawnWeaponDelegate, UnEquipTime, false);
			bIsWeaponChanging = true;
		}
		else
		{
			CurrentArmedWeapons[WeaponSlot]->Destroy();
			CurrentArmedWeapons[WeaponSlot] = nullptr;
		}
	}
}

void APSE_LYFE_Character4_Weapon::DeSpawnWeapon(const int8 WeaponSlot, int8 NewWeaponIndex)
{
	if (NewWeaponIndex != -1)
	{
		float EquipingTime = GetWeaponWithIndex(NewWeaponIndex)->EquipAnimation->SequenceLength;
		FTimerHandle WeaponChangeHandle;
		FTimerDelegate WeaponChangeDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_Character4_Weapon::SetCurrentWeapon, NewWeaponIndex);
		GetWorldTimerManager().SetTimer(WeaponChangeHandle, WeaponChangeDelegate, EquipingTime, false);
	}
	else
	{
		CurrentWeaponIndex = -1;
		bIsWeaponChanging = false;
	}
	CurrentArmedWeapons[WeaponSlot]->Destroy();
	CurrentArmedWeapons[WeaponSlot] = nullptr;
}

const int8 APSE_LYFE_Character4_Weapon::GetNextWeapon(const int8 CurrentRemovedWeapon) const
{
	for (int8 i = 0; i <= 2; i++)
	{
		if (CurrentRemovedWeapon != i && CurrentArmedWeapons[i])
		{
			return i;
		}
	}
	return -1;
}

void APSE_LYFE_Character4_Weapon::ChangeWeaponTo(int8 NewWeaponIndex)
{
	if (GetWeaponWithIndex(NewWeaponIndex) && NewWeaponIndex != CurrentWeaponIndex && bIsWeaponChanging == false)
	{
		ServerChangeWeaponTo(NewWeaponIndex);
	}
}

bool APSE_LYFE_Character4_Weapon::ServerChangeWeaponTo_Validate(int8 NewWeaponIndex)
{
	return true;
}

void APSE_LYFE_Character4_Weapon::ServerChangeWeaponTo_Implementation(int8 NewWeaponIndex)
{
	if (GetWeaponWithIndex(NewWeaponIndex) && NewWeaponIndex != CurrentWeaponIndex && bIsWeaponChanging == false)
	{
		bIsWeaponChanging = true;
		Client_UnEquipWeapon(CurrentWeaponIndex, NewWeaponIndex);
		const float UnEquipingTime = GetCurrentWeapon()->UnEquipAnimation->SequenceLength;
		const float EquipingTime = GetWeaponWithIndex(NewWeaponIndex)->UnEquipAnimation->SequenceLength;
		FTimerHandle WeaponChangeFinishHandle;
		FTimerDelegate WeaponChangeFinishDelegate = FTimerDelegate::CreateUObject(this, &APSE_LYFE_Character4_Weapon::ChangeWeaponFinish, NewWeaponIndex);
		GetWorldTimerManager().SetTimer(WeaponChangeFinishHandle, WeaponChangeFinishDelegate, (EquipingTime + UnEquipingTime), false);
	}
}

void APSE_LYFE_Character4_Weapon::ChangeWeaponFinish(int8 NewWeaponIndex)
{
	SetCurrentWeapon(NewWeaponIndex);
	bIsWeaponChanging = false;
}

APSE_LYFE_BaseWeapon* APSE_LYFE_Character4_Weapon::GetCurrentWeapon() const
{
	return GetWeaponWithIndex(CurrentWeaponIndex);
}

APSE_LYFE_BaseWeapon* APSE_LYFE_Character4_Weapon::GetWeaponWithIndex(int8 Index) const
{
	if (CurrentArmedWeapons.IsValidIndex(Index))
	{
		return CurrentArmedWeapons[Index];
	}
	else
	{
		return nullptr;
	}
}

void APSE_LYFE_Character4_Weapon::StartWeaponFire()
{
	if (CanFire() && GetCurrentWeapon())
	{
		//	if (GetCurrentWeapon()->CurrentState != EWeaponState::Passive)
		//	{
		//		GetCurrentWeapon()->StartFire();
		//	}
	}
}


void APSE_LYFE_Character4_Weapon::StopWeaponFire()
{
	if (GetCurrentWeapon())
	{
		//	if (GetCurrentWeapon()->IsA(APSE_LYFE_AutoRWeapon::StaticClass()))
		//	{
		//		GetCurrentWeapon()->StopFire();
		//	}
	}
}


void APSE_LYFE_Character4_Weapon::StartWeaponReload()
{
	/*APSE_LYFE_BaseWeapon* TempWeapon = GetCurrentWeapon();
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
	}*/
}

void APSE_LYFE_Character4_Weapon::CancleWeaponReload()
{
	/*
	APSE_LYFE_BaseWeapon* TempWeapon = GetCurrentWeapon();
	if (TempWeapon->IsA(APSE_LYFE_ReloadableWeapon::StaticClass()))
	{
	APSE_LYFE_ReloadableWeapon* ReloadableWeapon = Cast<APSE_LYFE_ReloadableWeapon>(TempWeapon);
	if (ReloadableWeapon->CurrentState == EWeaponState::Reloading)
	{
	ReloadableWeapon->CancelReload();
	}
	}
	*/
}

bool APSE_LYFE_Character4_Weapon::CanFire()
{
	if (!bIsWeaponChanging && GrenadeComp->CurrentGrenadeState == EGrenadeState::Null)
	{
		return false;
	}
	return true;
}


void APSE_LYFE_Character4_Weapon::ChangeWeaponTo1()
{
	ChangeWeaponTo(0);
}

void APSE_LYFE_Character4_Weapon::ChangeWeaponTo2()
{
	ChangeWeaponTo(1);
}

void APSE_LYFE_Character4_Weapon::LeftClickPressed()
{
	bWantsToFire = true;
	if (GetCurrentWeapon()) // Also check weapon firing condition here
	{
		if (bIsSprinting == true)
		{
			EndSprint();
		}
		if (CanFire())
		{
			StartWeaponFire();
		}
	}
}

void APSE_LYFE_Character4_Weapon::LeftClickReleased()
{

	bWantsToFire = false;
	/*StopWeaponFire();*/
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

const TArray<APSE_LYFE_BaseWeapon*> APSE_LYFE_Character4_Weapon::GetAllWeapons() const// Not needed any more
{
	return CurrentArmedWeapons;
}

FVector APSE_LYFE_Character4_Weapon::LeftHandWeaponAttachmentLocation()
{
	if (GetCurrentWeapon())
	{
		FVector WorldWeaponLocation = GetCurrentWeapon()->Mesh3P->GetSocketLocation("LeftHandAttachmentLocation");
		return GetMesh()->ComponentToWorld.InverseTransformPosition(WorldWeaponLocation);
	}

	return FVector(0, 0, 0);
}

void APSE_LYFE_Character4_Weapon::SetWeaponIKDetection(const bool bIKDetection)
{
	AnimBP_bIKLeftHand = bIKDetection;
}

void APSE_LYFE_Character4_Weapon::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APSE_LYFE_Character4_Weapon, CurrentWeaponIndex);
	DOREPLIFETIME(APSE_LYFE_Character4_Weapon, CurrentArmedWeapons);
	DOREPLIFETIME(APSE_LYFE_Character4_Weapon, TemporaryWeapon);
}

