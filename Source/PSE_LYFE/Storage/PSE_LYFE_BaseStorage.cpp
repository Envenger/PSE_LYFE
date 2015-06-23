// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_BaseStorage.h"


// Sets default values
APSE_LYFE_BaseStorage::APSE_LYFE_BaseStorage()
{
	StorageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StorageMesh"));
	RootComponent = StorageMesh;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	StorageSize = 20;
}

void APSE_LYFE_BaseStorage::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetStorageBagSize(StorageSize);
}

void APSE_LYFE_BaseStorage::SetStorageBagSize(const int16 StorageSize)
{
	int32 NoOfRows = FMath::CeilToInt((float)StorageSize / 5);
	if (NoOfRows < 5)
	{
		NoOfRows = 5;
	}
	StorageDisplaySize = FStorageLoc(NoOfRows, 5);
	Storage.AddUninitialized(NoOfRows, 5);
	Storage.InitializeArray(StorageBase, StorageSize);
}

void APSE_LYFE_BaseStorage::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APSE_LYFE_BaseStorage, StorageBase);
}

