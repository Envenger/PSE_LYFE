// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "TestChar.h"


// Sets default values
ATestChar::ATestChar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetSkeletalMesh(Mesh1);
}

// Called when the game starts or when spawned
void ATestChar::BeginPlay()
{
	Super::BeginPlay();
	if (Mesh1 && Mesh2)
	{
		FTimerHandle MeshChangeHandle;
		GetWorldTimerManager().SetTimer(MeshChangeHandle, this, &ATestChar::ChangeMesh, 1.5, true);
	}

}

// Called every frame
void ATestChar::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ATestChar::ChangeMesh()
{
	if (GetMesh()->SkeletalMesh == Mesh1)
	{
		GetMesh()->SetSkeletalMesh(Mesh2);
	}
	else if (GetMesh()->SkeletalMesh == Mesh2)
	{
		GetMesh()->SetSkeletalMesh(Mesh1);
	}
}

// Called to bind functionality to input
void ATestChar::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

