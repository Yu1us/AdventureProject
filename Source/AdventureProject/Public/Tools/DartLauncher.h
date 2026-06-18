// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippableToolBase.h"
#include "DartLauncher.generated.h"

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREPROJECT_API ADartLauncher : public AEquippableToolBase
{
	GENERATED_BODY()
	
public:

	virtual void Use() override;

	virtual void BindInputAction(const UInputAction* InputToBind) override;

};
