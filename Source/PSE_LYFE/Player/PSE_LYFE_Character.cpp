// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_Character.h"
#include "Player/Storage/PSE_LYFE_PlayerInventory.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "UnrealNetwork.h"

APSE_LYFE_Character::APSE_LYFE_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraAimLocation = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("CameraAimLocation"));
	CameraAimLocation->RelativeLocation = FVector(-250, 0, 42);
	CameraAimLocation->ArrowColor = FColor(FLinearColor(1, 0, 0));
	CameraAimLocation->AttachTo(RootComponent);


	CameraNonAimLocation = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("CameraNonAimLocation"));
	CameraNonAimLocation->ArrowColor = FColor(FLinearColor(0, 1, 0));
	CameraNonAimLocation->AttachTo(RootComponent);

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->RelativeLocation = CameraAimLocation->RelativeLocation;

	// Create a follow camera
	FollowCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	

	AnimBP_CrouchStandAlpha = 0;
	CrouchState = ECrouchState::Null;
	CrouchingDuration = 0.3;

	bIsInventoryOpen = false;
	MotionDirection = FVector2D(0, 0);

	bIsTurning90 = false;
	TurnAngle = 0;
	TurnSpeed = 0;

	AnimBP_TurningLeft = false;
	AnimBP_TurningRight = false;

	CameraAimCoeffcient = 0;
	CameraAimingTime = 0.25;
	bIsTryingCameraAim = true;
}

void APSE_LYFE_Character::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// Set up gameplay key bindings
	check(InputComponent);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &APSE_LYFE_Character::StartCrouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &APSE_LYFE_Character::EndCrouch);
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	InputComponent->BindAction("Use", IE_Released, this, &APSE_LYFE_Character::PickInventoryItem);
	InputComponent->BindAction("OpenInventory", IE_Pressed, this, &APSE_LYFE_Character::OpenInventory);

	InputComponent->BindAction("SecondaryFire", IE_Pressed, this, &APSE_LYFE_Character::RightClickPressed);
	InputComponent->BindAction("SecondaryFire", IE_Released, this, &APSE_LYFE_Character::RightClickReleased);

	InputComponent->BindAxis("MoveForward", this, &APSE_LYFE_Character::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &APSE_LYFE_Character::MoveRight);

	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &APSE_LYFE_Character::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &APSE_LYFE_Character::LookUpAtRate);
}

void APSE_LYFE_Character::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	NonAimCameraLocation = CameraNonAimLocation->RelativeLocation;
	NonAimCameraRotation = CameraNonAimLocation->RelativeRotation;
	AimCameraLocation = CameraAimLocation->RelativeLocation;
	AimCameraRotation = CameraAimLocation->RelativeRotation;

	UpdateCamera(NonAimCameraLocation, NonAimCameraRotation);
}

void APSE_LYFE_Character::BeginPlay()
{
	Super::BeginPlay();
	if (Role == ROLE_Authority)
	{
		InventoryPtr = GetWorld()->SpawnActor<APSE_LYFE_PlayerInventory>(DefaultInventoryClass);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, "Spawned");
		InventoryPtr->SetOwningPawn(this);
	}
	WalkSpeedCache = GetCharacterMovement()->MaxWalkSpeed;
}

void APSE_LYFE_Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AnimBP_MoveSpeed = GetMovementComponent()->Velocity.Size();

	FRotator PCRot = GetControlRotation();
	//PCRot.Normalize();

	FRotator PawnRot = GetActorRotation();
	//PawnRot.Normalize();
	FRotator RotDiffrence = PCRot - PawnRot;
	RotDiffrence.Normalize();
	
	if (AnimBP_MoveSpeed > 1)
	{
		AnimBP_AimYaw = 0;
		PawnRot.Yaw = PCRot.Yaw;
		SetActorRotation(PawnRot);
		
		/*if (AnimBP_TurningRight == true)
		{
			AnimBP_TurningRight = false;
		}
		if (AnimBP_TurningLeft == true)
		{
			AnimBP_TurningLeft = false;
		}*/
	}
	else
	{
		float RotationDiffrence = PawnRot.Yaw - PCRot.Yaw;
		if (FMath::Abs(RotDiffrence.Yaw) <= 90)
		{
			AnimBP_AimYaw = RotDiffrence.Yaw;
			/*if (AnimBP_TurningRight == true)
			{
				AnimBP_TurningRight = false;
			}
			if (AnimBP_TurningLeft == true)
			{
				AnimBP_TurningLeft = false;
			}*/
		}
		else if (FMath::Abs(RotDiffrence.Yaw) > 90)
		{
			if (bIsTurning90 == false)
			{
				Turn90(RotDiffrence.Yaw);
			}
			/*
			float RotChange = 90 * FMath::Sign(RotDiffrence.Yaw);
			AnimBP_AimYaw = RotChange;
			PawnRot.Yaw = PCRot.Yaw - RotChange;
			SetActorRotation(PawnRot);
			if (AnimBP_TurningRight == true)
			{
				AnimBP_TurningRight = false;
			}
			if (AnimBP_TurningLeft == true)
			{
				AnimBP_TurningLeft = false;
			}
			if (RotChange > 0)
			{
				if (AnimBP_TurningRight == false)
				{
					AnimBP_TurningRight = true;
				}
			}
			else
			{
				if (AnimBP_TurningLeft == false)
				{
					AnimBP_TurningLeft = true;
				}
			}*/
		}
	}
	if (bIsTurning90 == true)
	{
	if (!FMath::IsNearlyZero(TurnAngle))
		{
			float DeltaRot = FMath::Sign(TurnAngle)*(FMath::Min((TurnSpeed * DeltaSeconds), FMath::Abs(TurnAngle)));
			TurnAngle -= DeltaRot;
			FRotator PawnRotation = GetActorRotation();
			PawnRotation.Yaw += DeltaRot;
			AnimBP_AimYaw -= DeltaRot;
			SetActorRotation(PawnRotation);
		}
		else
		{
			bIsTurning90 = false;
		}
	}
	if (Role != ROLE_Authority)
	{
	if (IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Blue, "Controller = " + FString::SanitizeFloat(PCRot.Yaw));
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Blue, "Pawn = " + FString::SanitizeFloat(PawnRot.Yaw));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Green, "Controller = " + FString::SanitizeFloat(PCRot.Yaw));
		GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Green, "Pawn = " + FString::SanitizeFloat(PawnRot.Yaw));
	}
	}
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
	CalculateCameraAim(DeltaSeconds);
}

void APSE_LYFE_Character::RightClickPressed()
{
	bIsTryingCameraAim = true;
}

void APSE_LYFE_Character::RightClickReleased()
{
	bIsTryingCameraAim = false;
}

void APSE_LYFE_Character::CalculateCameraAim(const float DeltaSeconds)
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
	if (CameraAimCoeffcient != NewCameraAimCoeffcient)
	{
		CameraAimCoeffcient = NewCameraAimCoeffcient;
		FVector DeltaLocation;
		DeltaLocation.X = FMath::Lerp(NonAimCameraLocation.X, AimCameraLocation.X, CameraAimCoeffcient);
		DeltaLocation.Y = FMath::Lerp(NonAimCameraLocation.Y, AimCameraLocation.Y, CameraAimCoeffcient);
		DeltaLocation.Z = FMath::Lerp(NonAimCameraLocation.Z, AimCameraLocation.Z, CameraAimCoeffcient);
		//GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds*20, FColor::Blue,
		//	FString::SanitizeFloat(DeltaLocation.X) + " " +
		//	FString::SanitizeFloat(DeltaLocation.Y) + " " +
		//	FString::SanitizeFloat(DeltaLocation.Z));
		const FRotator DeltaRotation = FMath::Lerp(NonAimCameraRotation, AimCameraRotation, CameraAimCoeffcient);
		UpdateCamera(DeltaLocation, DeltaRotation);

		GetFollowCamera()->FieldOfView = FMath::Lerp(90, 75, CameraAimCoeffcient);
	}
}

void APSE_LYFE_Character::UpdateCamera(const FVector CameraLocation, const FRotator CameraRotation)
{
	GetCameraBoom()->TargetArmLength = -CameraLocation.X;
	GetCameraBoom()->SetRelativeLocation(FVector(0, 0, CameraLocation.Z));
	GetCameraBoom()->SocketOffset = FVector(0, CameraLocation.Y, 0);// CameraLocation.Z);

	GetFollowCamera()->RelativeRotation = CameraRotation;
}

void APSE_LYFE_Character::Turn90(const float RotDiffrence)
{
	if (RotDiffrence > 0)
	{
		PlayAnimMontage(TurnRight90Animation, 1);
		bIsTurning90 = true;
		TurnAngle += RotDiffrence;
		TurnSpeed = FMath::Abs(RotDiffrence)/(TurnRight90Animation->SequenceLength);// 90 degree turn in sequence time
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::SanitizeFloat(RotDiffrence));
	}
	else if (RotDiffrence < 0)
	{
		PlayAnimMontage(TurnLeft90Animation, 1);
		bIsTurning90 = true;
		TurnAngle += RotDiffrence;
		TurnSpeed = FMath::Abs(RotDiffrence) / TurnLeft90Animation->SequenceLength;// 90 degree turn in sequence time
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::SanitizeFloat(RotDiffrence));
	}
}


void APSE_LYFE_Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APSE_LYFE_Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APSE_LYFE_Character::MoveForward(float Value)
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

void APSE_LYFE_Character::MoveRight(float Value)
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

void APSE_LYFE_Character::StartCrouch()
{
	if (CrouchState != ECrouchState::StartCrouch)
	{
		CrouchState = ECrouchState::StartCrouch;
		ServerStartCrouch();
	}
}

bool APSE_LYFE_Character::ServerStartCrouch_Validate()
{
	return true;
}

void APSE_LYFE_Character::ServerStartCrouch_Implementation()
{
	if (CrouchState != ECrouchState::StartCrouch)
	{
		CrouchState = ECrouchState::StartCrouch;
	}
}

void APSE_LYFE_Character::EndCrouch()
{
	if (CrouchState != ECrouchState::EndCrouch)
	{
		CrouchState = ECrouchState::EndCrouch;
		ServerEndCrouch();
	}
}

bool APSE_LYFE_Character::ServerEndCrouch_Validate()
{
	return true;
}

void APSE_LYFE_Character::ServerEndCrouch_Implementation()
{
	if (CrouchState != ECrouchState::EndCrouch)
	{
		CrouchState = ECrouchState::EndCrouch;
	}
}

void APSE_LYFE_Character::CalculateCrouch(const float DeltaSeconds)
{

	if (!GetCharacterMovement()->IsFalling())
	{
		if (CrouchState == ECrouchState::StartCrouch)
		{
			if (AnimBP_CrouchStandAlpha < 1)
			{
				AnimBP_CrouchStandAlpha = FMath::Min((AnimBP_CrouchStandAlpha + (1 / CrouchingDuration*DeltaSeconds)), 1.0f);
				GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(WalkSpeedCache, GetCharacterMovement()->MaxWalkSpeedCrouched, AnimBP_CrouchStandAlpha);
			}
			else if (AnimBP_CrouchStandAlpha == 1)// Crouch Complete
			{
				GetCharacterMovement()->MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
			}
		}
		else if (CrouchState == ECrouchState::EndCrouch)
		{
			if (AnimBP_CrouchStandAlpha > 0)
			{
				AnimBP_CrouchStandAlpha = FMath::Max((AnimBP_CrouchStandAlpha - ((1 / CrouchingDuration)*DeltaSeconds)), 0.0f);
				GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(WalkSpeedCache, GetCharacterMovement()->MaxWalkSpeedCrouched, AnimBP_CrouchStandAlpha);
			}
			else if (AnimBP_CrouchStandAlpha == 0)// Crouch End competly
			{
				GetCharacterMovement()->MaxWalkSpeed = WalkSpeedCache;
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

bool APSE_LYFE_Character::ServerUpdateMoveDirection_Validate(float NewMoveDirection)
{
	return true;
}

void APSE_LYFE_Character::ServerUpdateMoveDirection_Implementation(float NewMoveDirection)
{
	AnimBP_MoveDirection = NewMoveDirection;
}


bool APSE_LYFE_Character::ServerSetAimOffset_Validate(float NewAimOffset)
{
	return true;
}

void APSE_LYFE_Character::ServerSetAimOffset_Implementation(float NewAimOffset)
{
	AnimBP_AimPitch = NewAimOffset;
}


bool APSE_LYFE_Character::ServerSetAim_Validate(FVector Origin, FVector Direction)
{
	return true;
}

void APSE_LYFE_Character::ServerSetAim_Implementation(FVector Origin, FVector Direction)
{
	ViewOrigin = Origin;
	ViewDirection = Direction;
}

/////////////////////////////////////////////////////////
// Inventory

void APSE_LYFE_Character::OpenInventory()
{
	if (bIsInventoryOpen == false)
	{
		bIsInventoryOpen = true;
		if (CharacterHUD)
		{
			CharacterHUD->CreateInventory();
		}
	}
	else
	{
		bIsInventoryOpen = false;
	}
}

void APSE_LYFE_Character::PickInventoryItem()
{
	FVector TraceStart = ViewOrigin;
	FVector TraceEnd = ViewOrigin + (1000 * ViewDirection);
	FCollisionQueryParams TraceParams(TEXT("CoverTrace"), true, this);
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, ECC_PhysicsBody, TraceParams);
	if (Hit.bBlockingHit && ((Hit.ImpactPoint - GetActorLocation()).Size() < 300))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->IsA(APSE_LYFE_Item_Base::StaticClass()) && InventoryPtr)
			{
				ServerPickInventoryItem();
			}
		}
	}
}

bool APSE_LYFE_Character::ServerPickInventoryItem_Validate()
{
	return true;
}

void APSE_LYFE_Character::ServerPickInventoryItem_Implementation()
{
	FVector TraceStart = ViewOrigin;
	FVector TraceEnd = ViewOrigin + (1000 * ViewDirection);
	FCollisionQueryParams TraceParams(TEXT("CoverTrace"), true, this);
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, ECC_PhysicsBody, TraceParams);
	if (Hit.bBlockingHit && ((Hit.ImpactPoint - GetActorLocation()).Size() < 300))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->IsA(APSE_LYFE_Item_Base::StaticClass()) && InventoryPtr)
			{
				APSE_LYFE_Item_Base* TempItem = Cast<APSE_LYFE_Item_Base>(Hit.GetActor());
				InventoryPtr->AttemptPickItem(TempItem);
			}
		}
	}
}

/////////////////////////////////////////////////////////



void APSE_LYFE_Character::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character, AnimBP_CrouchStandAlpha, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character, AnimBP_MoveDirection, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character, InventoryPtr, COND_OwnerOnly);
}

