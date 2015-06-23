// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character5_Cover.h"
#include "PSE_LYFE_CoverVolume.h"

APSE_LYFE_CoverVolume::APSE_LYFE_CoverVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bColored = true;
	BrushColor.R = 90;
	BrushColor.G = 255;
	BrushColor.B = 255;
	BrushColor.A = 255;
}

void APSE_LYFE_CoverVolume::ReceiveActorBeginOverlap(AActor* OtherActor)
{
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (Role == ROLE_Authority)
	{
		if (OtherActor->IsA(APSE_LYFE_Character5_Cover::StaticClass()))
		{
			APSE_LYFE_Character5_Cover* CoverCharacter = Cast<APSE_LYFE_Character5_Cover>(OtherActor);
			CoverCharacter->CoveredCounter += 1;
		}
	}
}

void APSE_LYFE_CoverVolume::ReceiveActorEndOverlap(AActor* OtherActor)
{
	Super::ReceiveActorEndOverlap(OtherActor);
	if (Role == ROLE_Authority)
	{
		if (OtherActor->IsA(APSE_LYFE_Character5_Cover::StaticClass()))
		{
			APSE_LYFE_Character5_Cover* CoverCharacter = Cast<APSE_LYFE_Character5_Cover>(OtherActor);
			CoverCharacter->CoveredCounter -= 1;
		}
	}
}
