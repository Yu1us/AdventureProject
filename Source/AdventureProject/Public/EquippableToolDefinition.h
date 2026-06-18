// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "EquippableToolDefinition.generated.h"

class AEquippableToolBase;
class UInputMappingContext;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREPROJECT_API UEquippableToolDefinition : public UItemDefinition
{
	GENERATED_BODY()

public:
	// The tool actor associated with this item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AEquippableToolBase> ToolAsset;

	// Create and return a copy of this item
	virtual UEquippableToolDefinition* CreateItemCopy(UObject* Outer) const override;

};
