// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_Character0_Base.h"


// Sets default values
APSE_LYFE_Character0_Base::APSE_LYFE_Character0_Base(const FObjectInitializer& ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	CameraAimLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraAimLocation"));
	CameraAimLocation->RelativeLocation = FVector(-250, 0, 42);
	CameraAimLocation->ArrowColor = FColor(FLinearColor(1, 0, 0));
	CameraAimLocation->AttachTo(RootComponent);

	CameraNonAimLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("CameraNonAimLocation"));
	CameraNonAimLocation->ArrowColor = FColor(FLinearColor(0, 1, 0));
	CameraNonAimLocation->AttachTo(RootComponent);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->AttachTo(RootComponent);
	CameraBoom->RelativeLocation = CameraAimLocation->RelativeLocation;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	CameraAimCoeffcient = 0;
	CameraAimingTime = 0.25;
	bIsTryingCameraAim = false;
}

void APSE_LYFE_Character0_Base::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	NonAimCameraLocation = CameraNonAimLocation->RelativeLocation;
	NonAimCameraRotation = CameraNonAimLocation->RelativeRotation;
	AimCameraLocation = CameraAimLocation->RelativeLocation;
	AimCameraRotation = CameraAimLocation->RelativeRotation;

	UpdateCamera(NonAimCameraLocation, NonAimCameraRotation);
}

void APSE_LYFE_Character0_Base::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAction("RightClick", IE_Pressed, this, &APSE_LYFE_Character0_Base::RightClickPressed);
	InputComponent->BindAction("RightClick", IE_Released, this, &APSE_LYFE_Character0_Base::RightClickReleased);
}

// Called every frame
void APSE_LYFE_Character0_Base::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	CalculateCameraAim(DeltaTime);
}

void APSE_LYFE_Character0_Base::CalculateCameraAim(const float DeltaSeconds)
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
	if (CameraAimCoeffcient != NewCameraAimCoeffcient)
	{
		CameraAimCoeffcient = NewCameraAimCoeffcient;
		FVector DeltaLocation;
		DeltaLocation.X = FMath::Lerp(NonAimCameraLocation.X, AimCameraLocation.X, CameraAimCoeffcient);
		DeltaLocation.Y = FMath::Lerp(NonAimCameraLocation.Y, AimCameraLocation.Y, CameraAimCoeffcient);
		DeltaLocation.Z = FMath::Lerp(NonAimCameraLocation.Z, AimCameraLocation.Z, CameraAimCoeffcient);

		const FRotator DeltaRotation = FMath::Lerp(NonAimCameraRotation, AimCameraRotation, CameraAimCoeffcient);

		UpdateCamera(DeltaLocation, DeltaRotation);

		GetFollowCamera()->FieldOfView = FMath::Lerp(90, 75, CameraAimCoeffcient);
	}
}

void APSE_LYFE_Character0_Base::UpdateCamera(const FVector CameraLocation, const FRotator CameraRotation)
{
	GetCameraBoom()->TargetArmLength = -CameraLocation.X;
	GetCameraBoom()->SetRelativeLocation(FVector(0, 0, CameraLocation.Z));
	GetCameraBoom()->SocketOffset = FVector(0, CameraLocation.Y, 0);// CameraLocation.Z);

	GetFollowCamera()->RelativeRotation = CameraRotation;
}

void APSE_LYFE_Character0_Base::RightClickPressed()
{
	bIsTryingCameraAim = true;
}

void APSE_LYFE_Character0_Base::RightClickReleased()
{
	bIsTryingCameraAim = false;
}

