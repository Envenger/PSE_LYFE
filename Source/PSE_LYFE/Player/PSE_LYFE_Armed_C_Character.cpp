// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Cover/PSE_LYFE_CoverVolume.h"
#include "UnrealNetwork.h"
#include "PSE_LYFE_Armed_C_Character.h"

APSE_LYFE_Armed_C_Character::APSE_LYFE_Armed_C_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HighTraceLoc = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("HighTrace"));
	HighTraceLoc->RelativeLocation = FVector(0,0,50);
	LowTraceLoc = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("LowTrace"));
	LowTraceLoc->RelativeLocation = FVector(0, 0, 15);
	HighTraceLoc->AttachTo(RootComponent);
	LowTraceLoc->AttachTo(RootComponent);

	CoveredCounter = 0;


}

void APSE_LYFE_Armed_C_Character::BeginPlay()
{
	if (Role == ROLE_Authority)
	{
		one = GetWorld()->SpawnActor<AActor>(ActorClass);
		two = GetWorld()->SpawnActor<AActor>(ActorClass);
	}
	Super::BeginPlay();
}

void APSE_LYFE_Armed_C_Character::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Role == ROLE_Authority)
	{
		if (CoveredCounter > 0)
		{
			float RotationAngle =  FMath::DegreesToRadians(GetCapsuleComponent()->GetComponentRotation().Yaw);
			FVector2D TraceEndAdd((100 * FMath::Cos(RotationAngle)), (100 * FMath::Sin(RotationAngle)));

			FVector FirstTraceStart = HighTraceLoc->GetComponentLocation();
			FVector FirstTraceEnd = FVector((FirstTraceStart.X + TraceEndAdd.X), (FirstTraceStart.Y + TraceEndAdd.Y), FirstTraceStart.Z);
			FVector SecondTraceStart = LowTraceLoc->GetComponentLocation();
			FVector SecondTraceEnd = FVector((SecondTraceStart.X + TraceEndAdd.X), (SecondTraceStart.Y + TraceEndAdd.Y), SecondTraceStart.Z);

			one->SetActorLocation(FirstTraceEnd);
			two->SetActorLocation(SecondTraceEnd);

			FCollisionQueryParams TraceParams(TEXT("CoverTrace"), true, this);
			FHitResult Hit1(ForceInit);
			FHitResult Hit2(ForceInit);

			GetWorld()->LineTraceSingle(Hit1, FirstTraceStart, FirstTraceEnd, ECC_PhysicsBody, TraceParams);
			GetWorld()->LineTraceSingle(Hit2, SecondTraceStart, SecondTraceEnd, ECC_PhysicsBody, TraceParams);
			if (Hit1.bBlockingHit)
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Green, "1");
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, "1");
			}
			if (Hit2.bBlockingHit)
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Green, "2");
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaSeconds, FColor::Red, "2");
			}
		}
	}
}

void APSE_LYFE_Armed_C_Character::ReceiveActorBeginOverlap(AActor* OtherActor)
{
	Super::ReceiveActorBeginOverlap(OtherActor);
	if (Role == ROLE_Authority)
	{
		if (OtherActor->IsA(APSE_LYFE_CoverVolume::StaticClass()))
		{
			CoveredCounter += 1;
		}
	}
}

void APSE_LYFE_Armed_C_Character::ReceiveActorEndOverlap(AActor* OtherActor)
{
	Super::ReceiveActorEndOverlap(OtherActor);
	if (Role == ROLE_Authority)
	{
		if (OtherActor->IsA(APSE_LYFE_CoverVolume::StaticClass()))
		{
			CoveredCounter -= 1;
		}
	}
}

void APSE_LYFE_Armed_C_Character::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APSE_LYFE_Armed_C_Character, CoveredCounter);
}

