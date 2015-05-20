// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Items/Equipments/PSE_LYFE_BaseBootsItem.h"
#include "Items/Equipments/PSE_LYFE_BaseBottomItem.h"
#include "Items/Equipments/PSE_LYFE_BaseGlovesItem.h"
#include "Items/Equipments/PSE_LYFE_BaseTopItem.h"
#include "Items/BackPack/PSE_LYFE_BackPackEquipment.h"
#include "PSE_LYFE_Character0_Base.generated.h"

USTRUCT()
struct FCharacterBodyComponent
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Hair)
	USkeletalMesh* HairMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Hair)
	UMaterial* HairMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Top)
	UMaterial* BodyMaterial;

	const bool IsValidComponent()
	{
		if (HairMesh && HairMaterial && BodyMaterial)
		{
			return true;
		}
		return false;
	}

	FCharacterBodyComponent()
	{}
};

USTRUCT()
struct FCharacterEquipmentStatus
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	bool bIsWearingTop;

	UPROPERTY()
	bool bIsWearingGloves;

	UPROPERTY()
	bool bIsWearingBottom;

	UPROPERTY()
	bool bIsWearingBoots;

	FCharacterEquipmentStatus()
	{
		bIsWearingTop = false;
		bIsWearingGloves = false;
		bIsWearingBottom = false;
		bIsWearingBoots = false;
	}

};

UCLASS()
class PSE_LYFE_API APSE_LYFE_Character0_Base : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	APSE_LYFE_Character0_Base(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

private:

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Hair;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Top;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Bottom;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* Boots;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BackPack;

	const bool InitializeCharacterSkeletalComponents();

public:

	const FName BackPackSocketName = "BackPackSocket";

///////////////////////////////////////////////////////
// Default Equipments

	UPROPERTY(EditDefaultsOnly, Category = DefaultCharacter)
	FCharacterBodyComponent DefaultBodyStruct;

	UPROPERTY(EditDefaultsOnly, Category = DefaultCharacter)
	FCharacterTopComponent DefaultTopStruct;

	UPROPERTY(EditDefaultsOnly, Category = DefaultCharacter)
	FCharacterGlovesComponent DefaultGlovesStruct;

	UPROPERTY(EditDefaultsOnly, Category = DefaultCharacter)
	FCharacterBootsComponent DefaultBootsStruct;

	UPROPERTY(EditDefaultsOnly, Category = DefaultCharacter)
	FCharacterBottomComponent DefaultBottomStruct;

///////////////////////////////////////////////////////
// Current Equipments


	UPROPERTY()
	FCharacterBodyComponent CurrentBodyStruct;

	UPROPERTY()
	FCharacterTopComponent CurrentTopStruct;

	UPROPERTY()
	FCharacterGlovesComponent CurrentGlovesStruct;

	UPROPERTY()
	FCharacterBootsComponent CurrentBootsStruct;

	UPROPERTY()
	FCharacterBottomComponent CurrentBottomStruct;
	
	const bool EquipItem(const FItemStruct ItemStruct);

	/** Also similar to wear default item */
	const bool UnEquipItem(EEquipmentSlotType EqipmentSlotType);

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* CameraAimLocation;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* CameraNonAimLocation;

public:

	/** 0 for non aim and 1 for aimed */
	float CameraAimCoeffcient;

	bool bIsTryingCameraAim;

	/** Time taken to shift aim */
	float CameraAimingTime;

	virtual void CalculateCameraAim(const float DeltaSeconds);

	FRotator NonAimCameraRotation;

	FVector NonAimCameraLocation;

	FRotator AimCameraRotation;

	FVector AimCameraLocation;

	void UpdateCamera(const FVector CameraLocation, const FRotator CameraRotation);

protected:

	void RightClickPressed();

	void RightClickReleased();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
};
