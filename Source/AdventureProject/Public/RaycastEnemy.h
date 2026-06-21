// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "RaycastEnemy.generated.h"

UENUM(BlueprintType)
enum class ERaycastState : uint8
{
	Aiming,     // Rotating toward player, waiting for line-of-sight
	Charging,   // Player in sight — showing red laser warning
	Firing,     // Instant raycast hit check
	Cooldown    // Pause before next cycle
};

/**
 * Raycast enemy: aims at the player, shows a red laser warning,
 * then fires a hitscan ray after a charge-up delay.
 * Damages the player on hit, cycles back to aiming on cooldown.
 */
UCLASS()
class ADVENTUREPROJECT_API ARaycastEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ARaycastEnemy();

	// Damage dealt per ray hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float RayDamage = 15.0f;

	// Seconds of red-laser warning before firing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float ChargeTime = 0.8f;

	// Seconds between shots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float RayCooldown = 2.0f;

	// Max range of the ray
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float RayRange = 3000.0f;

	// Distance to player that triggers the aim/attack cycle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Raycast")
	float AggroRange = 2000.0f;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	ERaycastState CurrentState = ERaycastState::Aiming;

	// Locked aim point committed when charging begins
	FVector FireTargetLocation = FVector::ZeroVector;

	// Timer accumulator for charging / cooldown
	float StateTimer = 0.0f;

	void TickAiming(float DeltaTime);
	void TickCharging(float DeltaTime);
	void TickFiring(float DeltaTime);
	void TickCooldown(float DeltaTime);
};
