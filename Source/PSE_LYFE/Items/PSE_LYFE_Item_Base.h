// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_Item_Base.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
namespace EItemType
{
	//Max of 256 entries per type, each entry gets associated with 0-255
	enum Type
	{
		StackableSingleConsumable 	UMETA(DisplayName = "StackableSingleConsumable"),
		MultiConsumable 	UMETA(DisplayName = "MultiConsumable"),
		Weapon 	UMETA(DisplayName = "Weapon"),
		Ammo	UMETA(DisplayName = "Ammo"),

		//
		Nothing		UMETA(Hidden),
	};
}

USTRUCT()
struct FItemStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	TSubclassOf<class APSE_LYFE_Item_Base> ItemClass;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	TEnumAsByte<EItemType::Type> ItemType;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	FSlateBrush ItemIcon;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	bool bStackable;

	/** Creates Item from the class properties */
	virtual void CreateItem(UWorld* World, FVector SpawnLocation = FVector(0, 0, 0), FRotator SpawnRotation = FRotator(0, 0, 0))
	{
	}

	FItemStruct(TSubclassOf<class APSE_LYFE_Item_Base> NewItemClass, TEnumAsByte<EItemType::Type> NewItemType, FSlateBrush  NewItemIcon, bool bIsStackable)
	{
		ItemClass = NewItemClass;
		ItemType = NewItemType;
		ItemIcon = NewItemIcon;
		bStackable = bIsStackable;
	}

	FItemStruct()
	{
	}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_Item_Base : public AActor
{
	GENERATED_BODY()
	
public:

	APSE_LYFE_Item_Base(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ItemProperties)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties)
	TEnumAsByte<EItemType::Type> ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties)
	FSlateBrush ItemIcon;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	TSubclassOf<class APSE_LYFE_Item_Base> ItemClass;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	bool bStackable;

	FItemStruct CreateStruct();
	
	
};
