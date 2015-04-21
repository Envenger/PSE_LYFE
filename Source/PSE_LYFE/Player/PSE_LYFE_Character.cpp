// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "Player/Inventory/PSE_LYFE_Inventory4_QuickSlots.h"
#include "PSE_LYFE_CMovementComponent.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Character.h"

APSE_LYFE_Character::APSE_LYFE_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPSE_LYFE_CMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
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
	CrouchState = ECrouchStateDE::Null;
	CrouchingDuration = 0.3;

	CrouchHeightDecrease = 40;
	CameraCrouchAlpha = AnimBP_CrouchStandAlpha;

	CurrentCrouchDecreasedHeight = 0;

	bIsSprinting = false;

	MotionDirection = FVector2D(0, 0);

	CameraAimCoeffcient = 0;
	CameraAimingTime = 0.25;
	bIsTryingCameraAim = false;
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

	InputComponent->BindAction("UseInventory", IE_Pressed, this, &APSE_LYFE_Character::UseInventory);


	InputComponent->BindAction("LeftClick", IE_Pressed, this, &APSE_LYFE_Character::LeftClickPressed);
	InputComponent->BindAction("LeftClick", IE_Released, this, &APSE_LYFE_Character::LeftClickReleased);
	InputComponent->BindAction("RightClick", IE_Pressed, this, &APSE_LYFE_Character::RightClickPressed);
	InputComponent->BindAction("RightClick", IE_Released, this, &APSE_LYFE_Character::RightClickReleased);

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
		InventoryPtr = GetWorld()->SpawnActor<APSE_LYFE_Inventory4_QuickSlots>(InventoryClass);
		InventoryPtr->SetOwningPawn(this);
	}
}

void APSE_LYFE_Character::OnRep_InventoryInitialize()
{
	if (InventoryPtr)
	{
		HUDStorageOwnerLink();
	}
}

void APSE_LYFE_Character::HUDStorageOwnerLink()
{
	if (GetController())
	{
		APlayerController* TempPC = Cast<APlayerController>(GetController());
		if (TempPC->GetHUD() && TempPC->GetHUD()->IsA(APSE_LYFE_TPSHUD::StaticClass()))
		{
			CharacterHUD = Cast<APSE_LYFE_TPSHUD>(TempPC->GetHUD());
			CharacterHUD->OwningCharacter = Cast<APSE_LYFE_ArmedCharacter>(this);
			CharacterHUD->InventoryPtr = InventoryPtr;
			InventoryPtr->CharacterHUD = CharacterHUD;
			CharacterHUD->CreateUI();
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "HUD Link Success");
		}
		else
		{
			FTimerHandle LinkDelayTimerHandle;
			GetWorldTimerManager().SetTimer(LinkDelayTimerHandle, this, &APSE_LYFE_Character::HUDStorageOwnerLink, 0.5, false);
			GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "HUD Link Failed1");
		}
	}
	else
	{
		FTimerHandle LinkDelayTimerHandle;
		GetWorldTimerManager().SetTimer(LinkDelayTimerHandle, this, &APSE_LYFE_Character::HUDStorageOwnerLink, 0.5, false);
		GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red, "HUD Link Failed2");
	}
}

void APSE_LYFE_Character::Tick(float DeltaSeconds)
{

	Super::Tick(DeltaSeconds);
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
	CalculateCameraAim(DeltaSeconds);
	/////////////////////////////////
}

bool APSE_LYFE_Character::ServerUpdateAimRotation_Validate(FRotator NewAimRotation)
{
	return true;
}

void APSE_LYFE_Character::ServerUpdateAimRotation_Implementation(FRotator NewAimRotation)
{
	AimRotation = NewAimRotation;
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

void APSE_LYFE_Character::UpdateCamera(const FVector CameraLocation, const FRotator CameraRotation)
{
	GetCameraBoom()->TargetArmLength = -CameraLocation.X;
	GetCameraBoom()->SetRelativeLocation(FVector(0, 0, CameraLocation.Z));
	GetCameraBoom()->SocketOffset = FVector(0, CameraLocation.Y, 0);// CameraLocation.Z);

	GetFollowCamera()->RelativeRotation = CameraRotation;
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
	if (CrouchState != ECrouchStateDE::StartCrouch)
	{
		CrouchState = ECrouchStateDE::StartCrouch;
		ServerStartCrouch();
	}
}

bool APSE_LYFE_Character::ServerStartCrouch_Validate()
{
	return true;
}

void APSE_LYFE_Character::ServerStartCrouch_Implementation()
{
	if (CrouchState != ECrouchStateDE::StartCrouch)
	{
		CrouchState = ECrouchStateDE::StartCrouch;
	}
}

void APSE_LYFE_Character::EndCrouch()
{
	if (CrouchState != ECrouchStateDE::EndCrouch)
	{
		CrouchState = ECrouchStateDE::EndCrouch;
		ServerEndCrouch();
	}
}

bool APSE_LYFE_Character::ServerEndCrouch_Validate()
{
	return true;
}

void APSE_LYFE_Character::ServerEndCrouch_Implementation()
{
	if (CrouchState != ECrouchStateDE::EndCrouch)
	{
		CrouchState = ECrouchStateDE::EndCrouch;
	}
}

void APSE_LYFE_Character::CalculateCrouch(const float DeltaSeconds)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		if (CrouchState == ECrouchStateDE::StartCrouch)
		{
			if (AnimBP_CrouchStandAlpha < 1)
			{
				AnimBP_CrouchStandAlpha = FMath::Min((AnimBP_CrouchStandAlpha + (1 / CrouchingDuration*DeltaSeconds)), 1.0f);
			}
		}
		else if (CrouchState == ECrouchStateDE::EndCrouch)
		{
			if (AnimBP_CrouchStandAlpha > 0)
			{
				AnimBP_CrouchStandAlpha = FMath::Max((AnimBP_CrouchStandAlpha - ((1 / CrouchingDuration)*DeltaSeconds)), 0.0f);
			}
			else if (AnimBP_CrouchStandAlpha == 0)// Crouch End competly
			{
				CrouchState = ECrouchStateDE::Null;
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

void APSE_LYFE_Character::StartSprint()
{
	if (bIsSprinting == false)
	{
		bIsSprinting = true;
		ServerStartSprint();
	}
}

bool APSE_LYFE_Character::ServerStartSprint_Validate()
{
	return true;
}

void APSE_LYFE_Character::ServerStartSprint_Implementation()
{
	if (bIsSprinting == false)
	{
		bIsSprinting = true;
	}
}

void APSE_LYFE_Character::EndSprint()
{
	if (bIsSprinting == true)
	{
		bIsSprinting = false;
		ServerStartSprint();
	}
}

bool APSE_LYFE_Character::ServerEndSprint_Validate()
{
	return true;
}

void APSE_LYFE_Character::ServerEndSprint_Implementation()
{
	if (bIsSprinting == true)
	{
		bIsSprinting = false;
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

void APSE_LYFE_Character::LeftClickPressed()
{}

void APSE_LYFE_Character::LeftClickReleased()
{}

void APSE_LYFE_Character::UseInventory()
{
	if (CharacterHUD)
	{
		if (CharacterHUD->bIsInventoryOpen == false)
		{
			CharacterHUD->CreateInventory();
		}
		else
		{
			CharacterHUD->CloseInventory();
		}
	}
}

void APSE_LYFE_Character::PickInventoryItem()
{
	FVector TraceStart = ViewOrigin;
	FVector TraceEnd = ViewOrigin + (500 * ViewDirection);
	FCollisionQueryParams TraceParams(TEXT("ItemPickTrace"), true, this);
	TraceParams.AddIgnoredActor(this);
	FHitResult Hit(ForceInit);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor(255, 150, 0), false, 10, 0, 6);
	GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, ECC_PhysicsBody, TraceParams);

	if (Hit.bBlockingHit && ((Hit.ImpactPoint - GetActorLocation()).Size() < 500))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->IsA(APSE_LYFE_BaseInventoryItem::StaticClass()) && InventoryPtr)
			{
				Server_PickInventoryItem();
			}
		}
	}
}

bool APSE_LYFE_Character::Server_PickInventoryItem_Validate()
{
	return true;
}

void APSE_LYFE_Character::Server_PickInventoryItem_Implementation()
{
	FVector TraceStart = ViewOrigin;
	FVector TraceEnd = ViewOrigin + (500 * ViewDirection);
	FCollisionQueryParams TraceParams(TEXT("ItemPickTrace"), true, this);
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, TraceStart, TraceEnd, ECC_PhysicsBody, TraceParams);
	if (Hit.bBlockingHit && ((Hit.ImpactPoint - GetActorLocation()).Size() < 500))
	{
		if (Hit.GetActor())
		{
			if (Hit.GetActor()->IsA(APSE_LYFE_BaseInventoryItem::StaticClass()) && InventoryPtr)
			{
				APSE_LYFE_BaseInventoryItem* TempItem = Cast<APSE_LYFE_BaseInventoryItem>(Hit.GetActor());
				FItemStruct ItemStructure = TempItem->CreateStruct();
				if (InventoryPtr->AddItem(ItemStructure))
				{
					TempItem->Destroy();
				}
			}
		}
	}
}

const FVector APSE_LYFE_Character::GetCharacterThrowLocation() const
{
	FVector ActorCurrentLocation = GetActorLocation();
	FVector RandomSpawnLoc;
	RandomSpawnLoc.X = FMath::FRandRange(-90, 90);
	RandomSpawnLoc.Y = FMath::FRandRange(-90, 90);
	RandomSpawnLoc.Z = FMath::FRandRange(45, 75);
	return (ActorCurrentLocation + RandomSpawnLoc);
}

/////////////////////////////////////////////////////////


void APSE_LYFE_Character::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character, AimRotation, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character, AnimBP_CrouchStandAlpha, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(APSE_LYFE_Character, AnimBP_MoveDirection, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(APSE_LYFE_Character, InventoryPtr, COND_OwnerOnly);

}

