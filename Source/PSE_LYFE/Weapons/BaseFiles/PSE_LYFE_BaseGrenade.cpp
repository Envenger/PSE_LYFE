// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "PSE_LYFE_BaseGrenade.h"


APSE_LYFE_BaseGrenade::APSE_LYFE_BaseGrenade()
{
	PrimaryActorTick.bCanEverTick = true;

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GMesh"));
	GrenadeMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RootComponent = GrenadeMesh;

	ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileComp->bShouldBounce = true;
	ProjectileComp->SetIsReplicated(true);
	ProjectileComp->InitialSpeed = 1500;
	ProjectileComp->BounceVelocityStopSimulatingThreshold = 15;
	//ProjectileComp->velo
	bReplicateMovement = true;
	bReplicates = true;

	ExplosionTimer = 3.5;
}

void APSE_LYFE_BaseGrenade::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle GrenadeExplosionTimer;
	GetWorldTimerManager().SetTimer(GrenadeExplosionTimer, this, &APSE_LYFE_BaseGrenade::Explosion, ExplosionTimer, false);
	if (Role == ROLE_Authority)
	{
		FTimerHandle ChangeCollisonTimer;
		GetWorldTimerManager().SetTimer(ChangeCollisonTimer, this, &APSE_LYFE_BaseGrenade::ChangeCollison, 0.35, false);
	}
}

void APSE_LYFE_BaseGrenade::ChangeCollison()
{
	GrenadeMesh->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
}

void APSE_LYFE_BaseGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void APSE_LYFE_BaseGrenade::Explosion()
{
	UParticleSystemComponent* ParticleTemp;
	ParticleTemp = UGameplayStatics::SpawnEmitterAttached
		(ExplosionParticleSystem,
		RootComponent,
		NAME_None,
		GetActorLocation(),
		GetActorRotation(),
		EAttachLocation::KeepWorldPosition,
		false);
	ParticleTemp->SetWorldScale3D(FVector(3.5));
	GrenadeMesh->DestroyComponent();
	FTimerHandle GrenadeDestoryTimer;
	GetWorldTimerManager().SetTimer(GrenadeDestoryTimer, this, &APSE_LYFE_BaseGrenade::DestoryActor, 5, false);
}

void APSE_LYFE_BaseGrenade::DestoryActor()
{
	Destroy();
}