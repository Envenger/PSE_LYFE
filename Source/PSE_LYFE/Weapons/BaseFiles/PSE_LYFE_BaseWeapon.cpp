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
	Mesh3P->SetVisibility(false);
	RootComponent = Mesh3P;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

}


void APSE_LYFE_BaseWeapon::SetOwningPawn(APSE_LYFE_Character4_Weapon* NewOwner)
{
	MyPawn = NewOwner;
	Instigator = NewOwner;
	SetOwner(NewOwner);
	//CurrentState = EWeaponState::Passive;
	AttachMeshToPawn();
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

	DOREPLIFETIME(APSE_LYFE_BaseWeapon, MyPawn);
}