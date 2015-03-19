// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "PSE_LYFE_BaseGrenade.h"
#include "PSE_LYFE_GrenadeComponent.generated.h"

UENUM(BlueprintType)
namespace EGrenadeState
{
	//Max of 256 entries per type, each entry gets associated with 0-255
	enum Type
	{
		Null 	UMETA(DisplayName = "None"),
		ThrowStart 	UMETA(DisplayName = "ThrowStart"),
		ThrowLoop 	UMETA(DisplayName = "ThrowLoop"),
		ThrowEnd	UMETA(DisplayName = "ThrowEnd"),

		//
		Nothing		UMETA(Hidden),
	};
}

UCLASS()
class PSE_LYFE_API UPSE_LYFE_GrenadeComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UPSE_LYFE_GrenadeComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(ReplicatedUsing=OnRep_GrenadeAnimation)
	TEnumAsByte<EGrenadeState::Type> CurrentGrenadeState;

	UFUNCTION()
	void OnRep_GrenadeAnimation();

	float ThrowStartTimer;

	float ThrowEndTimer;
	
	uint32 NoOfGrenades;

	float MaxForwardThrowVelocity;

	float MaxUpThrowVelocity;

	float MinForwardThrowVelocity;

	float MinUpThrowVelocity;

	void ClientThrowGrenadeStart();

	UFUNCTION(reliable, server, WithValidation)
	void ServerThrowGrenadeStart();
	
	void GrenadeLoopStart();
	
	void ClientThrowGrenadeFinish();

	UFUNCTION(reliable, server, WithValidation)
	void ServerThrowGrenadeFinish();

	void SpawnGrenade(float ForwardForce, float VerticalForce);

	UPROPERTY(EditDefaultsOnly, Category = GrenadeClass)
	TSubclassOf<APSE_LYFE_BaseGrenade> GrenadeClass;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* GrenadeStartAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* GrenadeLoopAnimation;

	UPROPERTY(EditDefaultsOnly, Category = Animation)
	UAnimMontage* GrenadeEndAnimation;
	/*
	void ClientThrowGrenadeCancel();

	UFUNCTION(reliable, server, WithValidation)
	void ServerThrowGrenadeCancel();
	*/

	//void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const;
};
