// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_BaseGrenade.generated.h"

/**
 * 
 */
UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseGrenade : public AActor
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_BaseGrenade();
	
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* ProjectileComp;

	virtual void BeginPlay() override;

	void ChangeCollison();

	virtual void Tick(float DeltaTime) override;

	FRotator ObjectRotationRate;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UStaticMeshComponent* GrenadeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	UParticleSystem* ExplosionParticleSystem;

	UPROPERTY(EditDefaultsOnly, Category = Grenade)
	float ExplosionTimer;

	void Explosion();

	void DestoryActor();

};
