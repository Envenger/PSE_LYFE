// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "PSE_LYFE_BackPackEquipment.generated.h"

UCLASS()
class PSE_LYFE_API APSE_LYFE_BackPackEquipment : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APSE_LYFE_BackPackEquipment();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ItemProperties)
		UStaticMeshComponent* BackPackMesh;
	
	
};
