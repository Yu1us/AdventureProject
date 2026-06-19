// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdventureCharacter.h"
#include "EnemyCharacter.generated.h"

/**
 * Enemy character base — reuses AdventureCharacter's tool/equipment system
 * but skips player-specific components (first-person camera, FP mesh, input).
 * Provides shared enemy infrastructure: health, death, player targeting.
 */
UCLASS()
class ADVENTUREPROJECT_API AEnemyCharacter : public AAdventureCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	// Visual color tint applied to the mesh (set per subclass or in editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FLinearColor EnemyTint = FLinearColor::White;

protected:
	virtual void BeginPlay() override;

	// Default enemy death: destroy self
	virtual void OnDeath_Implementation() override;

	// Get the player pawn (single-player shortcut)
	APawn* GetTargetPlayer() const;

	// Deal damage to the player
	void DamagePlayer(float Amount);
};
