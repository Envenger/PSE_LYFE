// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/PSE_LYFE_CMovementComponent.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Character1_Movement.h"

APSE_LYFE_Character1_Movement::APSE_LYFE_Character1_Movement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPSE_LYFE_CMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	AnimBP_CrouchStandAlpha = 0;
	CrouchState = ECrouchState::Null;
	CrouchingDuration = 0.3;

	CrouchHeightDecrease = 40;
	CameraCrouchAlpha = AnimBP_CrouchStandAlpha;

	CurrentCrouchDecreasedHeight = 0;

	bIsSprinting = false;

	MotionDirection = FVector2D(0, 0);
}

void APSE_LYFE_Character1_Movement::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("Crouch", IE_Pressed, this, &APSE_LYFE_Character1_Movement::StartCrouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &APSE_LYFE_Character1_Movement::EndCrouch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InputComponent->BindAxis("MoveForward", this, &APSE_LYFE_Character1_Movement::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APSE_LYFE_Character1_Movement::MoveRight);

	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APSE_LYFE_Character1_Movement::TurnAtRate);

	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APSE_LYFE_Character1_Movement::LookUpAtRate);
}

void APSE_LYFE_Character1_Movement::Tick(float DeltaSeconds)
{
	AnimBP_MoveSpeed = GetMovementComponent()->Velocity.Size();

	if (IsLocallyControlled())
	{
		if (MotionDirection.X == 0 && MotionDirection.Y == 0)
		{
			AnimBP_MoveDirection = 0;
		}
		else
		{
			if (MotionDirection.X == 0 || MotionDirection.Y == 0)
			{
				if (MotionDirection.Y == 0)
				{
					AnimBP_MoveDirection = (MotionDirection.X * -90) + 90;
				}
				else
				{
					AnimBP_MoveDirection = (90 * MotionDirection.Y);
				}
			}
			else
			{
				AnimBP_MoveDirection = (90 - (45 * MotionDirection.X)) * MotionDirection.Y;
			}
			ServerUpdateMoveDirection(AnimBP_MoveDirection);
			MotionDirection = FVector2D(0, 0);
		}
	}
	////////////////////////////////
	FRotator AimRotation = GetBaseAimRotation();
	AimRotation.Normalize();
	AnimBP_AimPitch = AimRotation.Pitch;
	////////////////////////////////
	CalculateCrouch(DeltaSeconds);
	/////////////////////////////////
	Super::Tick(DeltaSeconds);
}

void APSE_LYFE_Character1_Movement::CalculateCameraAim(const float DeltaSeconds)
{
	float NewCameraAimCoeffcient = CameraAimCoeffcient;
	if (bIsTryingCameraAim == true)
	{
		if (CameraAimCoeffcient < 1)
		{
			float DeltaIncrement = 1 / CameraAimingTime * DeltaSeconds;
			NewCameraAimCoeffcient = FMath::Min((NewCameraAimCoeffcient + DeltaIncrement), 1.0f);
		}
	}
	else
	{
		if (CameraAimCoeffcient > 0)
		{
			float DeltaIncrement = 1 / CameraAimingTime * DeltaSeconds;
			NewCameraAimCoeffcient = FMath::Max((NewCameraAimCoeffcient - DeltaIncrement), 0.0f);
		}
	}

	bool bDidAimDecrease = false;
	float NewCrouchHeightDifference = 0;
	if (CameraCrouchAlpha != AnimBP_CrouchStandAlpha)
	{
		CameraCrouchAlpha = AnimBP_CrouchStandAlpha;
		float NewCrouchDecreasedHeight = FMath::Lerp(0.0f, CrouchHeightDecrease, CameraCrouchAlpha);
		NewCrouchHeightDifference = CurrentCrouchDecreasedHeight - NewCrouchDecreasedHeight;
		CurrentCrouchDecreasedHeight = NewCrouchDecreasedHeight;

		bDidAimDecrease = true;
	}
	if (CameraAimCoeffcient != NewCameraAimCoeffcient)
	{
		CameraAimCoeffcient = NewCameraAimCoeffcient;
		FVector DeltaLocation;
		DeltaLocation.X = FMath::Lerp(NonAimCameraLocation.X, AimCameraLocation.X, CameraAimCoeffcient);
		DeltaLocation.Y = FMath::Lerp(NonAimCameraLocation.Y, AimCameraLocation.Y, CameraAimCoeffcient);
		DeltaLocation.Z = FMath::Lerp(NonAimCameraLocation.Z, AimCameraLocation.Z, CameraAimCoeffcient);

		DeltaLocation.Z -= CurrentCrouchDecreasedHeight;

		const FRotator DeltaRotation = FMath::Lerp(NonAimCameraRotation, AimCameraRotation, CameraAimCoeffcient);

		UpdateCamera(DeltaLocation, DeltaRotation);

		GetFollowCamera()->FieldOfView = FMath::Lerp(90, 75, CameraAimCoeffcient);
	}
	else if (bDidAimDecrease == true)
	{
		CameraCrouchAlpha = AnimBP_CrouchStandAlpha;

		FVector DeltaLocation = GetCameraBoom()->RelativeLocation;
		DeltaLocation.Z += NewCrouchHeightDifference;
		GetCameraBoom()->SetRelativeLocation(DeltaLocation);
	}

	if (IsLocallyControlled())
	{
		FVector TestLocation = GetCameraBoom()->RelativeLocation;
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Green, FString::SanitizeFloat(TestLocation.Z));
	}
}

bool APSE_LYFE_Character1_Movement::ServerUpdateAimRotation_Validate(FRotator NewAimRotation)
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerUpdateAimRotation_Implementation(FRotator NewAimRotation)
{
	AimRotation = NewAimRotation;
}

void APSE_LYFE_Character1_Movement::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APSE_LYFE_Character1_Movement::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APSE_LYFE_Character1_Movement::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		MotionDirection.X = Value;
	}
}

void APSE_LYFE_Character1_Movement::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		MotionDirection.Y = Value;
	}

}

///////////////////////////////////////////
// Crouch

void APSE_LYFE_Character1_Movement::StartCrouch()
{
	if (CrouchState != ECrouchState::StartCrouch)
	{
		CrouchState = ECrouchState::StartCrouch;
		ServerStartCrouch();
	}
}

bool APSE_LYFE_Character1_Movement::ServerStartCrouch_Validate()
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerStartCrouch_Implementation()
{
	if (CrouchState != ECrouchState::StartCrouch)
	{
		CrouchState = ECrouchState::StartCrouch;
	}
}

void APSE_LYFE_Character1_Movement::EndCrouch()
{
	if (CrouchState != ECrouchState::EndCrouch)
	{
		CrouchState = ECrouchState::EndCrouch;
		ServerEndCrouch();
	}
}

bool APSE_LYFE_Character1_Movement::ServerEndCrouch_Validate()
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerEndCrouch_Implementation()
{
	if (CrouchState != ECrouchState::EndCrouch)
	{
		CrouchState = ECrouchState::EndCrouch;
	}
}

void APSE_LYFE_Character1_Movement::CalculateCrouch(const float DeltaSeconds)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (CrouchState == ECrouchState::StartCrouch)
		{
			if (AnimBP_CrouchStandAlpha < 1)
			{
				AnimBP_CrouchStandAlpha = FMath::Min((AnimBP_CrouchStandAlpha + (1 / CrouchingDuration*DeltaSeconds)), 1.0f);
			}
		}
		else if (CrouchState == ECrouchState::EndCrouch)
		{
			if (AnimBP_CrouchStandAlpha > 0)
			{
				AnimBP_CrouchStandAlpha = FMath::Max((AnimBP_CrouchStandAlpha - ((1 / CrouchingDuration)*DeltaSeconds)), 0.0f);
			}
			else if (AnimBP_CrouchStandAlpha == 0)// Crouch End competly
			{
				CrouchState = ECrouchState::Null;
			}
		}
	}
	else if (AnimBP_CrouchStandAlpha != 0)
	{
		AnimBP_CrouchStandAlpha = 0;
	}
}

///////////////////////////////////////////
// Sprint

void APSE_LYFE_Character1_Movement::StartSprint()
{
	if (bIsSprinting == false)
	{
		bIsSprinting = true;
		ServerStartSprint();
	}
}

bool APSE_LYFE_Character1_Movement::ServerStartSprint_Validate()
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerStartSprint_Implementation()
{
	if (bIsSprinting == false)
	{
		bIsSprinting = true;
	}
}

void APSE_LYFE_Character1_Movement::EndSprint()
{
	if (bIsSprinting == true)
	{
		bIsSprinting = false;
		ServerStartSprint();
	}
}

bool APSE_LYFE_Character1_Movement::ServerEndSprint_Validate()
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerEndSprint_Implementation()
{
	if (bIsSprinting == true)
	{
		bIsSprinting = false;
	}
}

///////////////////////////////////////////

bool APSE_LYFE_Character1_Movement::ServerUpdateMoveDirection_Validate(float NewMoveDirection)
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerUpdateMoveDirection_Implementation(float NewMoveDirection)
{
	AnimBP_MoveDirection = NewMoveDirection;
}


bool APSE_LYFE_Character1_Movement::ServerSetAimOffset_Validate(float NewAimOffset)
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerSetAimOffset_Implementation(float NewAimOffset)
{
	AnimBP_AimPitch = NewAimOffset;
}


bool APSE_LYFE_Character1_Movement::ServerSetAim_Validate(FVector Origin, FVector Direction)
{
	return true;
}

void APSE_LYFE_Character1_Movement::ServerSetAim_Implementation(FVector Origin, FVector Direction)
{
	ViewOrigin = Origin;
	ViewDirection = Direction;
}

void APSE_LYFE_Character1_Movement::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character1_Movement, AimRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character1_Movement, AnimBP_CrouchStandAlpha, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character1_Movement, AnimBP_MoveDirection, COND_SkipOwner);
}