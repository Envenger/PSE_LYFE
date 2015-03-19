// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Player/Storage/PSE_LYFE_PlayerInventory.h"
#include "Player/HUD/PSE_LYFE_TPSHUD.h"
#include "PSE_LYFE_Character.generated.h"

namespace ECrouchState
{
	enum Type
	{
		Null,
		StartCrouch,
		EndCrouch,
	};
}

UCLASS()
class PSE_LYFE_API APSE_LYFE_Character : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:

	APSE_LYFE_Character(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** Used temporarily for server side hit detection should be replaced soon */
	UPROPERTY(replicated)
	FVector ViewOrigin;

	/** Used temporarily for server side hit detection should be replaced soon */
	UPROPERTY(replicated)
	FVector ViewDirection;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAim(FVector Origin, FVector Direction);

	/** Direction in which the character moves to control animations */
	FVector2D MotionDirection;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = AnimBP)
	float AnimBP_MoveDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimBP)
	float AnimBP_MoveSpeed;

	/** Updates the move offset to all other clients through the server(NEED NETWwRK OPTIMIZATION) */
	UFUNCTION(Server, reliable, WithValidation)
	void ServerUpdateMoveDirection(float NewMoveDirection);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimBP)
	bool AnimBP_TurningLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimBP)
	bool AnimBP_TurningRight;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* TurnRight90Animation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* TurnLeft90Animation;

	void Turn90(const float RotDiffrence);

	bool bIsTurning90;

	float TurnAngle;

	float TurnSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimBP)
	float AnimBP_AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AnimBP)
	float AnimBP_AimYaw;

///////////////////////////////////////////
// Crouch

	/** A temporary storage used to store the walkspeed while crouching */
	float WalkSpeedCache;

	/** Has value off 0 for normal and 1 for crouched */
	UPROPERTY(VisibleAnywhere, replicated, BlueprintReadOnly, Category = AnimBP)
	float AnimBP_CrouchStandAlpha;

	void StartCrouch();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartCrouch();

	void EndCrouch();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndCrouch();

	ECrouchState::Type CrouchState;

	/** DUration taken from stand to crouch */
	float CrouchingDuration;

	void CalculateCrouch(const float DeltaSeconds);

///////////////////////////////////////////

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAimOffset(float NewAimOffset);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

///////////////////////////////////////////////////////////////////
// Inventory

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<APSE_LYFE_PlayerInventory> DefaultInventoryClass;

	UPROPERTY(Replicated)
	APSE_LYFE_PlayerInventory* InventoryPtr;

	void PickInventoryItem();

	UFUNCTION(reliable, server, WithValidation)
	void ServerPickInventoryItem();

	void OpenInventory();

	bool bIsInventoryOpen;

	UPROPERTY()
	APSE_LYFE_TPSHUD* CharacterHUD;

///////////////////////////////////////////////////////////////////
// Camera change

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

	void CalculateCameraAim(const float DeltaSeconds);

	FRotator NonAimCameraRotation;

	FVector NonAimCameraLocation;

	FRotator AimCameraRotation;

	FVector AimCameraLocation;

	void UpdateCamera(const FVector CameraLocation, const FRotator CameraRotation);

protected:

	void RightClickPressed();

	void RightClickReleased();

///////////////////////////////////////////////////////////////////


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
};