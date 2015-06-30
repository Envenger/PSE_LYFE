// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_HitImpact.generated.h"

USTRUCT()
struct FDecalData
{
	GENERATED_USTRUCT_BODY()

	/** material */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	UMaterial* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category = Decal)
	float LifeSpan;

	/** defaults */
	FDecalData()
		: DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_HitImpact : public AActor
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_HitImpact(const FObjectInitializer& ObjectInitializer);

	/** spawn effect */
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category = Surface)
	FHitResult SurfaceHit;

	UPROPERTY(EditDefaultsOnly, Category = Defaults)
	struct FDecalData DefaultDecal;	
	
	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UParticleSystem* ImpactFX;

	/** spawned component for muzzle FX */
	UPROPERTY()
	UParticleSystemComponent* ImpactPSC;
};
