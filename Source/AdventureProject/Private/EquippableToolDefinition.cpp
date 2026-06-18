// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableToolDefinition.h"

UEquippableToolDefinition* UEquippableToolDefinition::CreateItemCopy(UObject* Outer) const
{
	// If we need an Outer, use GetTransientPackage
	if (!Outer)
	{
		Outer = GetTransientPackage();
	}

	return DuplicateObject<UEquippableToolDefinition>(this, Outer);
}