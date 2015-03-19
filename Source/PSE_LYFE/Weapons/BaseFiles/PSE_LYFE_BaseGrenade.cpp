// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_BaseGrenade.h"


APSE_LYFE_BaseGrenade::APSE_LYFE_BaseGrenade(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GrenadeMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("GMesh"));
	GrenadeMesh->SetSimulatePhysics(true);
	RootComponent = GrenadeMesh;

	bReplicateMovement = true;
	bReplicates = true;

	ExplosionTimer = 5;
}

void APSE_LYFE_BaseGrenade::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(this, &APSE_LYFE_BaseGrenade::Explosion, ExplosionTimer, false);
	}
	GetWorldTimerManager().SetTimer(this, &APSE_LYFE_BaseGrenade::Visible, 0.06, false);
}

void APSE_LYFE_BaseGrenade::Visible()
{
	GrenadeMesh->SetVisibility(true);
}

void APSE_LYFE_BaseGrenade::Explosion()
{
	Destroy();
}