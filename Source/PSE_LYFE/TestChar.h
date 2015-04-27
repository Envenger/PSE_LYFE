// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "TestChar.generated.h"

UCLASS()
class PSE_LYFE_API ATestChar : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestChar();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ChangeMesh();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	USkeletalMesh* Mesh1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh)
	USkeletalMesh* Mesh2;

	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	
	
};
