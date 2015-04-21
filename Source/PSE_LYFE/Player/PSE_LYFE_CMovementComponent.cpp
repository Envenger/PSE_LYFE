// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "PSE_LYFE_CMovementComponent.h"

float UPSE_LYFE_CMovementComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();
	APSE_LYFE_Character4_Weapon* OwnerCharacter = Cast<APSE_LYFE_Character4_Weapon>(PawnOwner);
	if (OwnerCharacter)
	{
		if (OwnerCharacter->CrouchState != ECrouchState::Null)
		{
			return FMath::Lerp(MaxSpeed, MaxWalkSpeedCrouched, OwnerCharacter->AnimBP_CrouchStandAlpha);
		}
		else if (OwnerCharacter->bIsSprinting == true)
		{
			return MaxWalkSpeedSprint;
		}
	}

	return MaxSpeed;
}


/*	if (CameraAimCoeffcient != NewCameraAimCoeffcient)
	{
		CameraAimCoeffcient = NewCameraAimCoeffcient;
		FVector DeltaLocation;
		DeltaLocation.X = FMath::Lerp(NonAimCameraLocation.X, AimCameraLocation.X, CameraAimCoeffcient);
		DeltaLocation.Y = FMath::Lerp(NonAimCameraLocation.Y, AimCameraLocation.Y, CameraAimCoeffcient);
		DeltaLocation.Z = FMath::Lerp(NonAimCameraLocation.Z, AimCameraLocation.Z, CameraAimCoeffcient);
		if (CameraCrouchAlpha != AnimBP_CrouchStandAlpha)
		{
			CameraCrouchAlpha = AnimBP_CrouchStandAlpha;

			float NewCrouchDecreasedHeight = FMath::Lerp(0.0f, CrouchHeightDecrease, CameraCrouchAlpha);
			float NewCrouchHeightDifference = CurrentCrouchDecreasedHeight - NewCrouchDecreasedHeight;
			DeltaLocation.Z += NewCrouchHeightDifference;
			CurrentCrouchDecreasedHeight = NewCrouchDecreasedHeight;
		}
		DeltaLocation.Z -= CurrentCrouchDecreasedHeight;
		const FRotator DeltaRotation = FMath::Lerp(NonAimCameraRotation, AimCameraRotation, CameraAimCoeffcient);

		UpdateCamera(DeltaLocation, DeltaRotation);

		GetFollowCamera()->FieldOfView = FMath::Lerp(90, 75, CameraAimCoeffcient);
	}
	else if (CameraCrouchAlpha != AnimBP_CrouchStandAlpha)
	{
		CameraCrouchAlpha = AnimBP_CrouchStandAlpha;

		FVector DeltaLocation = GetCameraBoom()->RelativeLocation;
		float NewCrouchDecreasedHeight = FMath::Lerp(0.0f, CrouchHeightDecrease, CameraCrouchAlpha);
		float NewCrouchHeightDifference = CurrentCrouchDecreasedHeight - NewCrouchDecreasedHeight;
		DeltaLocation.Z += NewCrouchHeightDifference;
		CurrentCrouchDecreasedHeight = NewCrouchDecreasedHeight;
		GetCameraBoom()->SetRelativeLocation(DeltaLocation);
		}*/