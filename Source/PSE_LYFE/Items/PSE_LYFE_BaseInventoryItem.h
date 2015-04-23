// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PSE_LYFE_BaseInventoryItem.generated.h"

class APSE_LYFE_Character4_Weapon;

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EItemType : uint8
{
	BaseItem,
	UsableItem,
	StackableItem,
	StackableUsableItem,
	MultiUseItem,
	EquipableItem,
	//
	Nothing,
};

/** Sub class of equipable item */
UENUM(BlueprintType)
enum class EEquipmentSlotType : uint8
{
	Primary,
	Secondary,
	Melee,
	Backpack,
	Head,
	Chest,
	Legs,
	Boots,
	//
	Nothing,
};

USTRUCT()
struct FItemStruct
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = ItemProperties)
	TSubclassOf<class APSE_LYFE_BaseInventoryItem> ItemClass;

	UPROPERTY(EditDefaultsOnly, Category = ItemProperties)
		TArray<int32> ItemProperties;

	/** Creates Item from the class properties */
	virtual void CreateItem(UWorld* World, const FVector &SpawnLocation = FVector(0, 0, 0), const FRotator &SpawnRotation = FRotator(0, 0, 0)) const
	{
		World->SpawnActor(ItemClass, &SpawnLocation, &SpawnRotation);
	}

	const APSE_LYFE_BaseInventoryItem* GetDefaultItem() const
	{
		return ItemClass->GetDefaultObject<APSE_LYFE_BaseInventoryItem>();
	}

	/** check the item type before calling this function */
	const bool SetStackAmount(const int32 NewStackAmount)
	{
		if (ItemProperties.IsValidIndex(0))
		{
			ItemProperties[0] = NewStackAmount;
			return true;
		}
		else
		{
			return false;
		}
	}

	int32& GetStacks()
	{
		return ItemProperties[0];
	}

	void EmptyItem()
	{
		ItemClass = nullptr;
		ItemProperties.Empty();
	}

	FItemStruct(TSubclassOf<class APSE_LYFE_BaseInventoryItem> NewItemClass)
	{
		ItemClass = NewItemClass;
	}

	FItemStruct()
	{
		ItemClass = nullptr;
	}
};

UCLASS()
class PSE_LYFE_API APSE_LYFE_BaseInventoryItem : public AActor
{
	GENERATED_BODY()

public:

	// Sets default values for this actor's properties
	APSE_LYFE_BaseInventoryItem();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = ItemProperties)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemProperties)
	FSlateBrush ItemIcon;

	/** Only For multiuse item */
	UPROPERTY(EditAnywhere, Category = ItemProperties)
	int32 MaxUses;

	int32 CurrentUses;

	/** should be 1 for non stackable items */
	UPROPERTY(EditAnywhere, Category = ItemProperties)
	int32 MaxStacks;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category = ItemProperties)
	EEquipmentSlotType EquipmentSlotType;

	virtual void UseItem(APSE_LYFE_Character4_Weapon* UsingPawn) const;

	virtual void EquipItem() const;

	/** Item should be deleted after creating the function */
	virtual FItemStruct CreateStruct() const;


};
