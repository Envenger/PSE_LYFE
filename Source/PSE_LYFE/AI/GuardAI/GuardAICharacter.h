// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AI/AITargetPoint.h"
#include "GuardAICharacter.generated.h"

UENUM(BlueprintType)
enum class EGuardAIState : uint8
{
		Null,
		Patroling,
		Pursuing,
		Escaping,
};

UCLASS()
class PSE_LYFE_API AGuardAICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGuardAICharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	//UPROPERTY(EditAnywhere, Category = Behavior)
	//class UBehaviorTree* BotBehavior;
	
	UPROPERTY(EditInstanceOnly, Category = Patroling)
	TArray<AAITargetPoint*> AIGuardPoints;

	UPROPERTY(EditInstanceOnly, Category = Patroling)
	bool bChoseRandomPatrolPoints;

	UPROPERTY(EditInstanceOnly, Category = Patroling)
	float MinPatrolWaitTime;

	UPROPERTY(EditInstanceOnly, Category = Patroling)
	float MaxPatrolWaitTime;

	const AAITargetPoint* GetNextPatrolPoint() const;

	void GetNextPatrolPoint(const uint8 CurrentPatrolPointIndex, uint8& NewPatrolPointIndex, AAITargetPoint*& NewPatrolPoint) const;

	void GetClosestPatrolPoint(uint8& NewPatrolPointIndex, AAITargetPoint*& NewPatrolPoint) const;

///////////////////////////////////////////////////////
// Weapon

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = AnimBP)
	bool AnimBP_bHasEquipedWeapon;

public:

	void EquipWeapon();

	void UnEquipWeapon();

	UFUNCTION(BlueprintCallable, Category = AnimBP)
	void AnimBP_EquipFinishNotify();

	UFUNCTION(BlueprintCallable, Category = AnimBP)
	void AnimBP_UnEquipFinishNotify();


protected:

	EGuardAIState GuardAIState;

public:

	bool SetAIGuardState(EGuardAIState NewAIState);
};
