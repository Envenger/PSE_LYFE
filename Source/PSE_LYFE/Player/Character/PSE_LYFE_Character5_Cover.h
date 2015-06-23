// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/Character/PSE_LYFE_Character4_Weapon.h"
#include "PSE_LYFE_Character5_Cover.generated.h"

UENUM(BlueprintType)		
enum class ECoverState : uint8
{
		NoCover,
		StartHighCoverL,
		StartHighCoverR,
		HighCoverL,
		HighCoverR,
		HighCoverL2R,
		HighCoverR2L,
		EndHighCover,
};



UCLASS()
class PSE_LYFE_API APSE_LYFE_Character5_Cover : public APSE_LYFE_Character4_Weapon
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* HighTraceLoc;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UArrowComponent* LowTraceLoc;

public:

	APSE_LYFE_Character5_Cover();

	void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(Replicated)
	uint8 CoveredCounter;

	bool bLockCoverDetection;

	void ChangeCoverState(const ECoverState NewCoverState);

	/** Called for forwards/backward input */
	virtual void MoveForward(float Value) override;

	/** Called for side to side input */
	virtual void MoveRight(float Value) override;

	UFUNCTION(BlueprintCallable, Category = AnimBP)
	void AnimBP_StartHighCoverNotify();

	UFUNCTION(BlueprintCallable, Category = AnimBP)
	void AnimBP_FinishHighCoverNotify();

	virtual void AddControllerPitchInput(float Val) override;

	virtual void AddControllerYawInput(float Val) override;

	void RotateCharacter(const float CoverNormal, const float RotationTime);
	
private:

	bool bCharacterRotationEnabled;

	float AddingRotationYaw;

	float RotationMovementCounter;

	float RotationDesiredEndTime;

	void DeltaCharacterRotation(const float DeltaTime);

///////////////////////////////////////////////////////////////////
// Durations

	UPROPERTY(EditDefaultsOnly, Category = Cover)
	float EnterCoverDuration;

private:

	const ECoverState CheckEnterCover(float &CoverYaw) const;

	UPROPERTY(EditdefaultsOnly, Category = Cover)
	int16 CoverTraceDistance;
	
};
