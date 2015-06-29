// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Player/Inventory/PSE_LYFE_InventoryStructures.h"
#include "PSE_LYFE_BaseStorage.generated.h"

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseStorage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSE_LYFE_BaseStorage();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = StorageProperties)
	UStaticMeshComponent* StorageMesh;

	virtual void PostInitializeComponents() override;

	UPROPERTY(Replicated)
	TArray<FItemStruct> StorageBase;

	/** This stores the item structures pointers */
	UPROPERTY()
	FStorageArray Storage;

	/** Row, Col size of the storage */
	uint16 StorageSize;
	
	void SetStorageBagSize(const int16 StorageSize);

	FStorageLoc StorageDisplaySize;
	
};
