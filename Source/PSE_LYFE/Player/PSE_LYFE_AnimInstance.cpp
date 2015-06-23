// Fill out your copyright notice in the Description page of Project Settings.

#include "PSE_LYFE.h"
#include "Player/Character/PSE_LYFE_Character5_Cover.h"
#include "Runtime/Engine/Classes/Animation/AnimNode_StateMachine.h"
#include "PSE_LYFE_AnimInstance.h"

void UPSE_LYFE_AnimInstance::NativeInitializeAnimation()
{
	CoverState = ECoverState::NoCover;
	HandAttachmentLocation = FVector::ZeroVector;
	Super::NativeInitializeAnimation();
}

/*
const FName UPSE_LYFE_AnimInstance::GetCurrentStateName(int32 MachineIndex)
{
	if (UAnimBlueprintGeneratedClass* AnimBlueprintClass = Cast<UAnimBlueprintGeneratedClass>((UObject*)GetClass()))
	{
		if ((MachineIndex >= 0) && (MachineIndex < AnimBlueprintClass->AnimNodeProperties.Num()))
		{
			const int32 InstancePropertyIndex = AnimBlueprintClass->AnimNodeProperties.Num() - 1 - MachineIndex;

			UStructProperty* MachineInstanceProperty = AnimBlueprintClass->AnimNodeProperties[InstancePropertyIndex];
			checkSlow(MachineInstanceProperty->Struct->IsChildOf(FAnimNode_StateMachine::StaticStruct()));

			FAnimNode_StateMachine* MachineInstance = MachineInstanceProperty->ContainerPtrToValuePtr<FAnimNode_StateMachine>(this);

			return FName("g");
			//return MachineInstance->GetCurrentStateName();
		}
	}

	return NAME_None;
}*/

/*
#include "Runtime/Engine/Classes/Animation/AnimNode_StateMachine.h"

int32 GetCurrentState() const
{
return CurrentState;
}
*/