// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_BaseWeapon.generated.h"

class APSE_LYFE_Character4_Weapon;

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseWeapon : public AActor
{
	GENERATED_BODY()

public:

	APSE_LYFE_BaseWeapon(const FObjectInitializer& ObjectInitializer);

	/** weapon mesh : 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* Mesh3P;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UAnimMontage* UnEquipAnimation;

	UPROPERTY(EditDefaultsOnly, Category = WeaponProperties)
	UAnimMontage* EquipAnimation;

	/** set the weapon's owning pawn */
	void SetOwningPawn(APSE_LYFE_Character4_Weapon* AOwningCharacter);

	/** pawn owner */
	UPROPERTY(Transient, Replicated)
	APSE_LYFE_Character4_Weapon* MyPawn;

	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/*void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;



	



	////////////////////////////////////////////////////////////
	//EQuiping Logic and Properties


	*/

	


};
