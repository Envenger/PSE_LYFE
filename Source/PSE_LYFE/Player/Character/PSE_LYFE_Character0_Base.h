// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "PSE_LYFE_Character0_Base.generated.h"

UCLASS()
class PSE_LYFE_API APSE_LYFE_Character0_Base : public ACharacter
{
	GENERATED_BODY()

private:

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	APSE_LYFE_Character0_Base(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

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

	/** 0 for nonaim and 1 for aimed */
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
