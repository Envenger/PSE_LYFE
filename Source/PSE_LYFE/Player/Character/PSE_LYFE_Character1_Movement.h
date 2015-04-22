// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Character/PSE_LYFE_Character0_Base.h"
#include "PSE_LYFE_Character1_Movement.generated.h"

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
class PSE_LYFE_API APSE_LYFE_Character1_Movement : public APSE_LYFE_Character0_Base
{
	GENERATED_BODY()

public:

	APSE_LYFE_Character1_Movement(const FObjectInitializer& ObjectInitializer);
	
	void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	/** Used temporarily for server side hit detection should be replaced soon */
	UPROPERTY(replicated)
	FVector ViewOrigin;

	/** Used temporarily for server side hit detection should be replaced soon */
	UPROPERTY(replicated)
	FVector ViewDirection;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAim(FVector Origin, FVector Direction);
	bool ServerSetAim_Validate(FVector Origin, FVector Direction);
	void ServerSetAim_Implementation(FVector Origin, FVector Direction);


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

	/** Used to update client rotation to all clients */
	UPROPERTY(replicated)
	FRotator AimRotation;

	UFUNCTION(Server, unreliable, WithValidation)
	void ServerUpdateAimRotation(FRotator NewAimRotation);
	bool ServerUpdateAimRotation_Validate(FRotator NewAimRotation);
	void ServerUpdateAimRotation_Implementation(FRotator NewAimRotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = AnimBP)
	float AnimBP_AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = AnimBP)
	float AnimBP_AimYaw;

	///////////////////////////////////////////
	// Crouch

	/** Has value off 0 for normal and 1 for crouched */
	UPROPERTY(VisibleAnywhere, replicated, BlueprintReadOnly, Category = AnimBP)
	float AnimBP_CrouchStandAlpha;

	void StartCrouch();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartCrouch();
	bool ServerStartCrouch_Validate();
	void ServerStartCrouch_Implementation();

	void EndCrouch();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerEndCrouch();
	bool ServerEndCrouch_Validate();
	void ServerEndCrouch_Implementation();

	ECrouchState::Type CrouchState;

	/** DUration taken from stand to crouch */
	float CrouchingDuration;

	void CalculateCrouch(const float DeltaSeconds);

	void CalculateCameraAim(const float DeltaSeconds) override;

	/** Height decrease if the camera when crouching */
	UPROPERTY(Category = Crouch, EditDefaultsOnly)
	float CrouchHeightDecrease;

	/** A value of crouch stored by the camera to do camera height calculations */
	float CameraCrouchAlpha;

	/** A value of crouch stored by the camera to do camera height calculations */
	float CurrentCrouchDecreasedHeight;

	///////////////////////////////////////////
	// Sprint

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, replicated, Category = AnimBP)
	bool bIsSprinting;

	void StartSprint();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSprint();
	bool ServerStartSprint_Validate();
	void ServerStartSprint_Implementation();

	void EndSprint();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndSprint();
	bool ServerEndSprint_Validate();
	void ServerEndSprint_Implementation();

	///////////////////////////////////////////

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetAimOffset(float NewAimOffset);
	bool ServerSetAimOffset_Validate(float NewAimOffset);
	void ServerSetAimOffset_Implementation(float NewAimOffset);

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
};
