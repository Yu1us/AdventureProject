// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

class UItemDefinition;

// Defines the type of the item.
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Tool UMETA(DisplayName = "Tool"),
	Consumable UMETA(DisplayName = "Consumable")
};

// Contains textual data about the item.
USTRUCT(BlueprintType)
struct FItemText
{
	GENERATED_BODY()

	// The text name of the item.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	// The text description of the item.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

};

// Defines a basic item that can be used in a data table.
USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	// The ID name of this item for referencing in a table row.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FName ID;

	// The type of the item.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	EItemType ItemType;

	// Text struct including the item name and description.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	FItemText ItemText;

	// The Data Asset item definition associated with this item.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Data")
	TSoftObjectPtr<UItemDefinition> ItemBase;

};