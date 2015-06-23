// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "UnrealNetwork.h"
#include "Player/PSE_LYFE_AnimInstance.h"
#include "PSE_LYFE_Character5_Cover.h"

APSE_LYFE_Character5_Cover::APSE_LYFE_Character5_Cover()
{
	HighTraceLoc = CreateDefaultSubobject<UArrowComponent>(TEXT("HighTrace"));
	if (HighTraceLoc)
	{
		HighTraceLoc->RelativeLocation = FVector(0, 0, 50);
		HighTraceLoc->AttachTo(RootComponent);
	}
	

	LowTraceLoc = CreateDefaultSubobject<UArrowComponent>(TEXT("LowTrace"));
	if (LowTraceLoc)
	{
		LowTraceLoc->RelativeLocation = FVector(0, 0, 15);
		LowTraceLoc->AttachTo(RootComponent);
	}

	CoverTraceDistance = 55;
	bLockCoverDetection = false;

	CoveredCounter = 0;

	bCharacterRotationEnabled = false;
	AddingRotationYaw = 0;
	RotationDesiredEndTime = 0;
	RotationMovementCounter = 0;

	EnterCoverDuration = 0;
}

void APSE_LYFE_Character5_Cover::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void APSE_LYFE_Character5_Cover::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APSE_LYFE_Character5_Cover::BeginPlay()
{
	Super::BeginPlay();

	
		
}

void APSE_LYFE_Character5_Cover::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ECoverState"), true);
		
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, EnumPtr->GetEnumName((uint8)CoverAnimInstance->CoverState));

		if (CoveredCounter > 0 && CoverAnimInstance->CoverState == ECoverState::NoCover && bLockCoverDetection == false)
		{
			float CoverNormal;
			if (CheckEnterCover(CoverNormal) == ECoverState::StartHighCoverR)
			{
				CoverAnimInstance->CoverState = ECoverState::StartHighCoverR;
				FRotator ControllerRotation = GetControlRotation();
				
				float AddingYaw = CoverNormal - ControllerRotation.Yaw;
				if (AddingYaw > 180)
				{
					AddingYaw = -(AddingYaw - 180);
				}
				else if (AddingYaw < -180)
				{
					AddingYaw = -(AddingYaw + 180);
				}
				GetCameraBoom()->bUsePawnControlRotation = false;
				GetCameraBoom()->bInheritPitch = false;
				GetCameraBoom()->bInheritYaw = false;
				GetCameraBoom()->bInheritRoll = false;
				FRotator PCRotation = GetControlRotation();
				GetCameraBoom()->SetRelativeRotation(PCRotation);
				RotateCharacter(AddingYaw, 1.167);
			}
		} 
	}

	if (IsLocallyControlled())
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, (GetCameraBoom()->RelativeRotation + GetActorRotation()).ToString() + "    "
			+ GetActorRotation().ToString());
		if (bCharacterRotationEnabled)
		{
			DeltaCharacterRotation(DeltaTime);
		}
	}
	
}

const ECoverState APSE_LYFE_Character5_Cover::CheckEnterCover(float &CoverNormal) const
{
	float RotationAngle = FMath::DegreesToRadians(GetActorRotation().Yaw);
	FVector2D TraceEndAdd((CoverTraceDistance * FMath::Cos(RotationAngle)), (CoverTraceDistance * FMath::Sin(RotationAngle)));

	FVector FirstTraceStart = HighTraceLoc->GetComponentLocation();
	FVector FirstTraceEnd = FVector((FirstTraceStart.X + TraceEndAdd.X), (FirstTraceStart.Y + TraceEndAdd.Y), FirstTraceStart.Z);
	FVector SecondTraceStart = LowTraceLoc->GetComponentLocation();
	FVector SecondTraceEnd = FVector((SecondTraceStart.X + TraceEndAdd.X), (SecondTraceStart.Y + TraceEndAdd.Y), SecondTraceStart.Z);

	FCollisionQueryParams TraceParams(TEXT("CoverTrace"), true, this);
	FHitResult Hit1(ForceInit);
	FHitResult Hit2(ForceInit);

	GetWorld()->LineTraceSingle(Hit1, FirstTraceStart, FirstTraceEnd, ECC_PhysicsBody, TraceParams);
	GetWorld()->LineTraceSingle(Hit2, SecondTraceStart, SecondTraceEnd, ECC_PhysicsBody, TraceParams);

	if (Hit1.bBlockingHit && Hit2.bBlockingHit)
	{
		FRotator HitNormal = Hit1.Normal.Rotation();
		HitNormal.Yaw += 180;
		CoverNormal = HitNormal.GetDenormalized().Yaw;
		return ECoverState::StartHighCoverR;
	}
	else if (Hit2.bBlockingHit)
	{
		return ECoverState::NoCover;
	}
	else
	{
		return ECoverState::NoCover;
	}
}

void APSE_LYFE_Character5_Cover::MoveForward(float Value)
{
	if (CoverAnimInstance->CoverState == ECoverState::NoCover)
	{
		if (Value > 0)
		{
			bLockCoverDetection = false;
		}
		Super::MoveForward(Value);
	}
	else if (CoverAnimInstance->CoverState == ECoverState::HighCoverR || CoverAnimInstance->CoverState == ECoverState::HighCoverL)
	{
		if (Value < 0)
		{
			float NewCharacterYaw = GetCameraBoom()->RelativeRotation.Yaw - GetActorRotation().Yaw;

			RotateCharacter(NewCharacterYaw, 0.39);
			CoverAnimInstance->CoverState = ECoverState::EndHighCover;
		}
	}
}


void APSE_LYFE_Character5_Cover::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value))
	{
		if (CoverAnimInstance->CoverState == ECoverState::NoCover)
		{
			Super::MoveRight(Value);
		}
		else if (CoverAnimInstance->CoverState == ECoverState::HighCoverR)
		{
			if (Value > 0)
			{
				Super::MoveRight(Value);
			}
			else
			{
				CoverAnimInstance->CoverState = ECoverState::HighCoverR2L;
			}
		}
		else if (CoverAnimInstance->CoverState == ECoverState::HighCoverL)
		{
			if (Value < 0)
			{
				Super::MoveRight(Value);
			}
			else
			{
				CoverAnimInstance->CoverState = ECoverState::HighCoverL2R;
			}
		}
	}
}



void APSE_LYFE_Character5_Cover::AddControllerPitchInput(float Val)
{
	if (CoverAnimInstance->CoverState == ECoverState::NoCover)
	{
		Super::AddControllerPitchInput(Val);
	}
	else
	{
		float PitchMovement = (-1.75 * Val);
		APlayerController* PC = Cast<APlayerController>(GetController());
		PC->RotationInput.Pitch += PitchMovement;
		FRotator RelativeRotation = GetCameraBoom()->RelativeRotation;
		RelativeRotation.Pitch += PitchMovement;
		GetCameraBoom()->SetRelativeRotation(RelativeRotation);
	}
}

void APSE_LYFE_Character5_Cover::AddControllerYawInput(float Val)
{
	if (CoverAnimInstance->CoverState == ECoverState::NoCover)
	{
		Super::AddControllerYawInput(Val);
	}
	else if (CoverAnimInstance->CoverState == ECoverState::EndHighCover)
	{
		float YawMovement = (2.5 * Val);
		APlayerController* PC = Cast<APlayerController>(GetController());
		PC->RotationInput.Yaw += YawMovement;
		FRotator RelativeRotation = GetCameraBoom()->RelativeRotation;
		RelativeRotation.Yaw += YawMovement;
		GetCameraBoom()->SetRelativeRotation(RelativeRotation);
	}
	else
	{
		FRotator RelativeRotation = GetCameraBoom()->RelativeRotation;
		RelativeRotation.Yaw += (2.5 * Val);
		GetCameraBoom()->SetRelativeRotation(RelativeRotation);
	}
}

void APSE_LYFE_Character5_Cover::AnimBP_StartHighCoverNotify()
{
	CoverAnimInstance->CoverState = ECoverState::HighCoverR;
}


void APSE_LYFE_Character5_Cover::AnimBP_FinishHighCoverNotify()
{
	//GetController()->SetControlRotation(GetCameraBoom()->RelativeRotation - GetControlRotation());
	GetCameraBoom()->bUsePawnControlRotation = true;
	GetCameraBoom()->bInheritPitch = true;
	GetCameraBoom()->bInheritYaw = true;
	GetCameraBoom()->bInheritRoll = true;
	CoverAnimInstance->CoverState = ECoverState::NoCover;
	bLockCoverDetection = true;
}

void APSE_LYFE_Character5_Cover::RotateCharacter(const float NewYaw, const float RotationTime)
{
	bCharacterRotationEnabled = true;
	AddingRotationYaw = NewYaw;
	RotationDesiredEndTime = RotationTime;
	RotationMovementCounter = FMath::Abs(AddingRotationYaw);
}

void APSE_LYFE_Character5_Cover::DeltaCharacterRotation(const float DeltaTime)
{
	float DeltaRotation = (AddingRotationYaw*DeltaTime) / RotationDesiredEndTime;
	if (RotationMovementCounter > FMath::Abs(DeltaRotation))
	{
		RotationMovementCounter -= FMath::Abs(DeltaRotation);
	}
	else
	{
		DeltaRotation = FMath::Sign(DeltaRotation)*RotationMovementCounter;
		bCharacterRotationEnabled = false;
	}
	FRotator CurrentRotation = GetControlRotation();

	CurrentRotation.Yaw += DeltaRotation;
	GetController()->SetControlRotation(CurrentRotation);
}

void APSE_LYFE_Character5_Cover::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APSE_LYFE_Character5_Cover, CoveredCounter);

}