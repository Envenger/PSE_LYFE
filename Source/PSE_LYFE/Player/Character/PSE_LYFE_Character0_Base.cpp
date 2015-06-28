// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Items/Equipments/PSE_LYFE_BaseBootsItem.h"
#include "Items/Equipments/PSE_LYFE_BaseBottomItem.h"
#include "Items/Equipments/PSE_LYFE_BaseGlovesItem.h"
#include "Items/Equipments/PSE_LYFE_BaseTopItem.h"
#include "Items/BackPack/PSE_LYFE_BaseBackPackItem.h"
#include "Player/PSE_LYFE_AnimInstance.h"
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

	Top = GetMesh();// We don't use get GetMesh()
	Top->AlwaysLoadOnClient = true;
	Top->AlwaysLoadOnServer = true;
	Bottom = CreateDefaultSubobject<USkeletalMeshComponent>("BottomComponent");
	if (Bottom)
	{
		Bottom->AlwaysLoadOnClient = true;
		Bottom->AlwaysLoadOnServer = true;
		Bottom->bOwnerNoSee = false;
		Bottom->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
		Bottom->bCastDynamicShadow = true;
		Bottom->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Bottom->bChartDistanceFactor = true;
		Bottom->bGenerateOverlapEvents = false;

		// Top acts as the head, as well as the parent for both animation and attachment.
		Bottom->AttachParent = Top;
		Bottom->SetMasterPoseComponent(Top);
	}
	
	
	Hair = CreateDefaultSubobject<USkeletalMeshComponent>("HairComponent");
	if (Hair)
	{
		Hair->AlwaysLoadOnClient = true;
		Hair->AlwaysLoadOnServer = true;
		Hair->bOwnerNoSee = false;
		Hair->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
		Hair->bCastDynamicShadow = true;
		Hair->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Hair->bChartDistanceFactor = true;
		Hair->bGenerateOverlapEvents = false;

		// Top acts as the head, as well as the parent for both animation and attachment.
		Hair->AttachParent = Top;
		Hair->SetMasterPoseComponent(Top);
	}
	
	Boots = CreateDefaultSubobject<USkeletalMeshComponent>("BootsComponent");
	if (Boots)
	{
		Boots->AlwaysLoadOnClient = true;
		Boots->AlwaysLoadOnServer = true;
		Boots->bOwnerNoSee = false;
		Boots->MeshComponentUpdateFlag = EMeshComponentUpdateFlag::AlwaysTickPose;
		Boots->bCastDynamicShadow = true;
		Boots->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Boots->bChartDistanceFactor = true;
		Boots->bGenerateOverlapEvents = false;

		// Top acts as the head, as well as the parent for both animation and attachment.
		Boots->AttachParent = Top;
		Boots->SetMasterPoseComponent(Top);
	}

	BackPack = CreateDefaultSubobject<UStaticMeshComponent>("BackPack");
	if (BackPack)
	{
		BackPack->AlwaysLoadOnClient = true;
		BackPack->AlwaysLoadOnServer = true;
		BackPack->bOwnerNoSee = false;
		BackPack->bCastDynamicShadow = true;
		BackPack->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		BackPack->bGenerateOverlapEvents = false;
		BackPack->SetVisibility(false);

		BackPack->AttachTo(Top, BackPackSocketName);
	}

}

const bool APSE_LYFE_Character0_Base::InitializeCharacterSkeletalComponents()
{

	CurrentBodyStruct = DefaultBodyStruct;
	CurrentBootsStruct = DefaultBootsStruct;
	CurrentBottomStruct = DefaultBottomStruct;
	CurrentGlovesStruct = DefaultGlovesStruct;
	CurrentTopStruct = DefaultTopStruct;

	Top->SetSkeletalMesh(CurrentTopStruct.TopMesh);
	Top->SetMaterial(0, CurrentBodyStruct.BodyMaterial);
	Top->SetMaterial(1, CurrentGlovesStruct.GloveMaterial);
	Top->SetMaterial(2, CurrentTopStruct.TopMaterial);

	Bottom->SetSkeletalMesh(CurrentBottomStruct.BottomMesh);
	Bottom->SetMaterial(0, CurrentBottomStruct.BottomMaterial);
	
	Hair->SetSkeletalMesh(CurrentBodyStruct.HairMesh);
	Hair->SetMaterial(0, CurrentBodyStruct.HairMaterial);

	Boots->SetSkeletalMesh(CurrentBootsStruct.BootsMesh);
	Boots->SetMaterial(0, CurrentBootsStruct.BootsMaterial);

	return true;
}

const bool APSE_LYFE_Character0_Base::EquipItem(const FItemStruct ItemStruct)
{
	const APSE_LYFE_BaseInventoryItem* DefaultItem = ItemStruct.GetDefaultItem();
	if (DefaultItem->IsA(APSE_LYFE_BaseTopItem::StaticClass()))
	{
		const APSE_LYFE_BaseTopItem* TopItem = Cast<APSE_LYFE_BaseTopItem>(DefaultItem);
		GetMesh()->SetSkeletalMesh(TopItem->TopStruct.TopMesh);
		GetMesh()->SetMaterial(0, CurrentBodyStruct.BodyMaterial);
		GetMesh()->SetMaterial(1, CurrentGlovesStruct.GloveMaterial);
		GetMesh()->SetMaterial(2, TopItem->TopStruct.TopMaterial);
		CurrentTopStruct.TopMaterial = TopItem->TopStruct.TopMaterial;
		CurrentTopStruct.TopMesh = TopItem->TopStruct.TopMesh;
	}
	else if (DefaultItem->IsA(APSE_LYFE_BaseGlovesItem::StaticClass()))
	{
		const APSE_LYFE_BaseGlovesItem* GlovesItem = Cast<APSE_LYFE_BaseGlovesItem>(DefaultItem);
		Top->SetMaterial(1, GlovesItem->GlovesStruct.GloveMaterial);
		CurrentGlovesStruct.GloveMaterial = GlovesItem->GlovesStruct.GloveMaterial;
	}
	else if (DefaultItem->IsA(APSE_LYFE_BaseBottomItem::StaticClass()))
	{
		const APSE_LYFE_BaseBottomItem* BottomItem = Cast<APSE_LYFE_BaseBottomItem>(DefaultItem);
		Bottom->SetSkeletalMesh(BottomItem->BottomStruct.BottomMesh);
		Bottom->SetMaterial(0, BottomItem->BottomStruct.BottomMaterial);
		CurrentBottomStruct.BottomMesh = BottomItem->BottomStruct.BottomMesh;
		CurrentBottomStruct.BottomMaterial = BottomItem->BottomStruct.BottomMaterial;
	}
	else if (DefaultItem->IsA(APSE_LYFE_BaseBootsItem::StaticClass()))
	{
		const APSE_LYFE_BaseBootsItem* BootsItem = Cast<APSE_LYFE_BaseBootsItem>(DefaultItem);
		Boots->SetSkeletalMesh(BootsItem->BootsStruct.BootsMesh);
		Boots->SetMaterial(0, BootsItem->BootsStruct.BootsMaterial);
		CurrentBootsStruct.BootsMesh = BootsItem->BootsStruct.BootsMesh;
		CurrentBootsStruct.BootsMaterial = BootsItem->BootsStruct.BootsMaterial;
	}
	else if (DefaultItem->IsA(APSE_LYFE_BaseBackPackItem::StaticClass()))
	{
		const APSE_LYFE_BaseBackPackItem* BackPackItem = Cast<APSE_LYFE_BaseBackPackItem>(DefaultItem);
		if (!BackPack->IsVisible())
		{
			BackPack->SetVisibility(true);
		}
		BackPack->SetStaticMesh(BackPackItem->GetItemMesh()->StaticMesh);
	}
	else
	{
		return false;
	}
	return true;
}

const bool APSE_LYFE_Character0_Base::UnEquipItem(EEquipmentSlotType EqipmentSlotType)
{
	if (EqipmentSlotType == EEquipmentSlotType::Top)
	{
		CurrentTopStruct = DefaultTopStruct;
		GetMesh()->SetSkeletalMesh(CurrentTopStruct.TopMesh);
		GetMesh()->SetMaterial(2, CurrentTopStruct.TopMaterial);
	}
	else if (EqipmentSlotType == EEquipmentSlotType::Gloves)
	{
		CurrentGlovesStruct = DefaultGlovesStruct;
		Top->SetMaterial(1, CurrentGlovesStruct.GloveMaterial);
	}
	else if (EqipmentSlotType == EEquipmentSlotType::Bottom)
	{
		CurrentBottomStruct = DefaultBottomStruct;
		Bottom->SetSkeletalMesh(CurrentBottomStruct.BottomMesh);
		Bottom->SetMaterial(0, CurrentBottomStruct.BottomMaterial);
	}
	else if (EqipmentSlotType == EEquipmentSlotType::Boots)
	{
		CurrentBootsStruct = DefaultBootsStruct;
		Boots->SetSkeletalMesh(CurrentBootsStruct.BootsMesh);
		Boots->SetMaterial(0, CurrentBootsStruct.BootsMaterial);
	}
	else if (EqipmentSlotType == EEquipmentSlotType::Backpack)
	{
		BackPack->SetVisibility(false);
	}
	else
	{
		return true;
	}
	return true;
}

void APSE_LYFE_Character0_Base::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetMesh()->GetAnimInstance() && GetMesh()->GetAnimInstance()->IsA(UPSE_LYFE_AnimInstance::StaticClass()))
	{
		CoverAnimInstance = Cast<UPSE_LYFE_AnimInstance>(GetMesh()->GetAnimInstance());
	}

	NonAimCameraLocation = CameraNonAimLocation->RelativeLocation;
	NonAimCameraRotation = CameraNonAimLocation->RelativeRotation;
	AimCameraLocation = CameraAimLocation->RelativeLocation;
	AimCameraRotation = CameraAimLocation->RelativeRotation;

	UpdateCamera(NonAimCameraLocation, NonAimCameraRotation);

	InitializeCharacterSkeletalComponents();
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
	if (IsLocallyControlled())
	{
		CalculateCameraFinal(DeltaTime);
	}
}

void APSE_LYFE_Character0_Base::CalculateCameraFinal(const float DeltaTime)
{
	float NewCameraAimCoeffcient = CalculateCameraAimCoeffcient(DeltaTime);
	if (NewCameraAimCoeffcient != CameraAimCoeffcient)
	{
		CameraAimCoeffcient = NewCameraAimCoeffcient;
		FVector NewCameraLocation(FVector::ZeroVector);
		FRotator NewCameraRotation(FRotator::ZeroRotator);
		float NewFieldOfView = 0;

		CalculateCameraAimAttributes(DeltaTime, NewCameraLocation, NewCameraRotation, NewFieldOfView);

		UpdateCamera(NewCameraLocation, NewCameraRotation);
		GetFollowCamera()->FieldOfView = NewFieldOfView;
	}
}

const float APSE_LYFE_Character0_Base::CalculateCameraAimCoeffcient(const float DeltaSeconds) const
{
	if (bIsTryingCameraAim == true)
	{
		if (CameraAimCoeffcient < 1)
		{
			float DeltaIncrement = 1 / CameraAimingTime * DeltaSeconds;
			return FMath::Min((CameraAimCoeffcient + DeltaIncrement), 1.0f);
		}
		else
		{
			return 1;
		}
	}
	else
	{
		if (CameraAimCoeffcient > 0)
		{
			float DeltaIncrement = 1 / CameraAimingTime * DeltaSeconds;
			return FMath::Max((CameraAimCoeffcient - DeltaIncrement), 0.0f);
		}
		else
		{
			return 0;
		}
	}
}

void APSE_LYFE_Character0_Base::CalculateCameraAimAttributes(const float DeltaSeconds, FVector &NewCameraLocation, FRotator &NewCameraRotation, float & NewFieldOfView) const
{
	NewCameraLocation = FMath::Lerp(NonAimCameraLocation, AimCameraLocation, CameraAimCoeffcient);
	NewCameraRotation = FMath::Lerp(NonAimCameraRotation, AimCameraRotation, CameraAimCoeffcient);
	NewFieldOfView = FMath::Lerp(90, 75, CameraAimCoeffcient);
}

void APSE_LYFE_Character0_Base::UpdateCamera(const FVector CameraLocation, const FRotator CameraRotation)
{
	UpdatedCameraLocation = CameraLocation;
	UpdatedCameraRotation = CameraRotation;

	GetCameraBoom()->TargetArmLength = -CameraLocation.X;
	GetCameraBoom()->SetRelativeLocation(FVector(0, 0, CameraLocation.Z));
	GetCameraBoom()->SocketOffset = FVector(0, CameraLocation.Y, 0);// CameraLocation.Z);

	GetFollowCamera()->SetRelativeRotation(CameraRotation);
}

void APSE_LYFE_Character0_Base::RightClickPressed()
{
	bIsTryingCameraAim = true;
}

void APSE_LYFE_Character0_Base::RightClickReleased()
{
	bIsTryingCameraAim = false;
}

