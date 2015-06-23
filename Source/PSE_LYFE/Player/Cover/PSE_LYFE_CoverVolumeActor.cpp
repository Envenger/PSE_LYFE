// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character5_Cover.h"
#include "PSE_LYFE_CoverVolumeActor.h"

APSE_LYFE_CoverVolumeActor::APSE_LYFE_CoverVolumeActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	TriggerVolume = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, TEXT("Volume"));
	RootComponent = TriggerVolume;
}

void APSE_LYFE_CoverVolumeActor::ReceiveActorBeginOverlap(AActor* OtherActor)
{
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (Role == ROLE_Authority)
	{
		if (OtherActor->IsA(APSE_LYFE_Character5_Cover::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 9, FColor::Red, "Enter");
			APSE_LYFE_Character5_Cover* CoverCharacter = Cast<APSE_LYFE_Character5_Cover>(OtherActor);
			CoverCharacter->CoveredCounter += 1;
		}
	}
}

void APSE_LYFE_CoverVolumeActor::ReceiveActorEndOverlap(AActor* OtherActor)
{
	Super::ReceiveActorEndOverlap(OtherActor);
	if (Role == ROLE_Authority)
	{
		if (OtherActor->IsA(APSE_LYFE_Character5_Cover::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 9, FColor::Red, "out");
			APSE_LYFE_Character5_Cover* CoverCharacter = Cast<APSE_LYFE_Character5_Cover>(OtherActor);
			CoverCharacter->CoveredCounter -= 1;
		}
	}
}