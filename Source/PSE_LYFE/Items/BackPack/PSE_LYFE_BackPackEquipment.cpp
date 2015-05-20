// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_BackPackEquipment.h"


// Sets default values
APSE_LYFE_BackPackEquipment::APSE_LYFE_BackPackEquipment()
{

	BackPackMesh = CreateOptionalDefaultSubobject<UStaticMeshComponent>("BackPackMesh");
	RootComponent = BackPackMesh;
}

// Called when the game starts or when spawned
void APSE_LYFE_BackPackEquipment::BeginPlay()
{
	Super::BeginPlay();
	

}
