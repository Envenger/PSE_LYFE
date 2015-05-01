// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Inventory0_Base.h"


// Sets default values
APSE_LYFE_Inventory0_Base::APSE_LYFE_Inventory0_Base()
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;


}

// Called when the game starts or when spawned
void APSE_LYFE_Inventory0_Base::BeginPlay()
{
	Super::BeginPlay();
}

void APSE_LYFE_Inventory0_Base::SetOwningPawn(APSE_LYFE_Character4_Weapon* NewOwner)
{
	OwningPawn = NewOwner;
	Instigator = NewOwner;
	SetOwner(NewOwner);
}

void APSE_LYFE_Inventory0_Base::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Inventory0_Base, OwningPawn, COND_OwnerOnly);
}