// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "PSE_LYFE_Character4_Weapon.h"
#include "Storage/PSE_LYFE_BaseStorage.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Character2_Inventory.h"

void APSE_LYFE_Character2_Inventory::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	// Set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Use", IE_Released, this, &APSE_LYFE_Character2_Inventory::UseItem);
	InputComponent->BindAction("UseInventory", IE_Pressed, this, &APSE_LYFE_Character2_Inventory::UseInventory);
}

void APSE_LYFE_Character2_Inventory::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		InventoryPtr = GetWorld()->SpawnActor<APSE_LYFE_Inventory5_ExterStorage>(InventoryClass);
		if (this->IsA(APSE_LYFE_Character4_Weapon::StaticClass()))
		{
			InventoryPtr->SetOwningPawn(Cast<APSE_LYFE_Character4_Weapon>(this));
		}
	}
}

void APSE_LYFE_Character2_Inventory::OnRep_InventoryInitialize()
{
	if (InventoryPtr)
	{
		HUDStorageOwnerLink();
	}
}

void APSE_LYFE_Character2_Inventory::HUDStorageOwnerLink()
{
	
	if (GetController())
	{
		APlayerController* TempPC = Cast<APlayerController>(GetController());
		if (TempPC->GetHUD() && TempPC->GetHUD()->IsA(APSE_LYFE_TPSHUD::StaticClass()))
		{
			CharacterHUD = Cast<APSE_LYFE_TPSHUD>(TempPC->GetHUD());
			if (this->IsA(APSE_LYFE_Character4_Weapon::StaticClass()))
			{
				CharacterHUD->OwningCharacter = Cast<APSE_LYFE_Character4_Weapon>(this);
			}
			CharacterHUD->InventoryPtr = InventoryPtr;
			InventoryPtr->CharacterHUD = CharacterHUD;
			CharacterHUD->CreateUI();
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "HUD Link Success");
		}
		else
		{
			FTimerHandle LinkDelayTimerHandle;
			GetWorldTimerManager().SetTimer(LinkDelayTimerHandle, this, &APSE_LYFE_Character2_Inventory::HUDStorageOwnerLink, 0.5, false);
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "HUD Link Failed1");
		}
	}
	else
	{
		FTimerHandle LinkDelayTimerHandle;
		GetWorldTimerManager().SetTimer(LinkDelayTimerHandle, this, &APSE_LYFE_Character2_Inventory::HUDStorageOwnerLink, 0.5, false);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "HUD Link Failed2");
	}
}

void APSE_LYFE_Character2_Inventory::UseInventory()
{
	if (CharacterHUD)
	{
		if (InventoryPtr->InventoryState == EInventoryState::Close)
		{
			if (CharacterHUD->CreateInventoryUI())
			{
				InventoryPtr->InventoryState = EInventoryState::InventoryOpen;
			}
		}
		else
		{
			InventoryPtr->CloseInventory();
		}
	}
}

void APSE_LYFE_Character2_Inventory::UseItem()
{
	FVector TraceStart = ViewOrigin;
	FVector TraceEnd = ViewOrigin + (500 * ViewDirection);
	FCollisionQueryParams TraceParams(TEXT("ItemPickTrace"), true, this);
	TraceParams.AddIgnoredActor(this);
	FHitResult Hit(ForceInit);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor(255, 150, 0), false, 10, 0, 6);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_PhysicsBody, TraceParams);

	if (Hit.bBlockingHit && ((Hit.ImpactPoint - GetActorLocation()).Size() < 500))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->IsA(APSE_LYFE_BaseInventoryItem::StaticClass()) && InventoryPtr)
			{
				Server_PickInventoryItem();
			}
			if (Hit.GetActor()->IsA(APSE_LYFE_BaseStorage::StaticClass()) && InventoryPtr && InventoryPtr->InventoryState == EInventoryState::Close)
			{
				Server_OpenStorage();
			}
		}
	}
}

bool APSE_LYFE_Character2_Inventory::Server_PickInventoryItem_Validate()
{
	return true;
}

void APSE_LYFE_Character2_Inventory::Server_PickInventoryItem_Implementation()
{
	FVector TraceStart = ViewOrigin;
	FVector TraceEnd = ViewOrigin + (500 * ViewDirection);
	FCollisionQueryParams TraceParams(TEXT("ItemPickTrace"), true, this);
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_PhysicsBody, TraceParams);
	if (Hit.bBlockingHit && ((Hit.ImpactPoint - GetActorLocation()).Size() < 500))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->IsA(APSE_LYFE_BaseInventoryItem::StaticClass()) && InventoryPtr)
			{
				APSE_LYFE_BaseInventoryItem* TempItem = Cast<APSE_LYFE_BaseInventoryItem>(Hit.GetActor());
				FItemStruct ItemStructure = TempItem->CreateStruct();
				if (InventoryPtr->AddItem(ItemStructure))
				{
					TempItem->Destroy();
				}
			}
		}
	}
}

bool APSE_LYFE_Character2_Inventory::Server_OpenStorage_Validate()
{	
	return true;
}

void APSE_LYFE_Character2_Inventory::Server_OpenStorage_Implementation()
{
	FVector TraceStart = ViewOrigin;
	FVector TraceEnd = ViewOrigin + (500 * ViewDirection);
	FCollisionQueryParams TraceParams(TEXT("ItemPickTrace"), true, this);
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_PhysicsBody, TraceParams);
	if (Hit.bBlockingHit && ((Hit.ImpactPoint - GetActorLocation()).Size() < 500))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->IsA(APSE_LYFE_BaseStorage::StaticClass()) && InventoryPtr)
			{
				APSE_LYFE_BaseStorage* TracedStorage = Cast<APSE_LYFE_BaseStorage>(Hit.GetActor());
				InventoryPtr->OpenedStorage = TracedStorage;
			}
		}
	}
}

const FVector APSE_LYFE_Character2_Inventory::GetCharacterThrowLocation() const
{
	FVector ActorCurrentLocation = GetActorLocation();
	FVector RandomSpawnLoc;
	RandomSpawnLoc.X = FMath::FRandRange(-90, 90);
	RandomSpawnLoc.Y = FMath::FRandRange(-90, 90);
	RandomSpawnLoc.Z = FMath::FRandRange(45, 75);
	return (ActorCurrentLocation + RandomSpawnLoc);
}

void APSE_LYFE_Character2_Inventory::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character2_Inventory, InventoryPtr, COND_OwnerOnly);

}