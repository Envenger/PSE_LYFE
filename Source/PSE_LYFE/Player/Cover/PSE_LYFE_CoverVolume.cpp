// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
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


