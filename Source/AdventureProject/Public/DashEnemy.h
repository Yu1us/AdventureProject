// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "DashEnemy.generated.h"

UENUM(BlueprintType)
enum class EDashState : uint8
{
	Tracking,
	WindUp,
	Dashing,
	Recover
};

/**
 * Dash enemy: tracks player, winds up with a warning, then charges in a straight line.
 * Damages the player on contact, stuns itself on wall hit or reaching max distance.
 */
UCLASS()
class ADVENTUREPROJECT_API ADashEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ADashEnemy();

	// Distance to player that triggers the dash windup
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashRange = 600.0f;

	// Damage dealt on dash hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashDamage = 25.0f;

	// Speed of the dash (cm/s)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float DashSpeed = 2000.0f;

	// Seconds of warning before dashing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float WindUpTime = 0.5f;

	// Seconds of stun after dash ends
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float RecoverTime = 1.0f;

	// Max dash distance before auto-stopping
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash")
	float MaxDashDistance = 1500.0f;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	EDashState CurrentState = EDashState::Tracking;

	// Locked dash direction (set during WindUp)
	FVector DashDirection = FVector::ZeroVector;

	// Position where dash started (for distance tracking)
	FVector DashStartLocation = FVector::ZeroVector;

	// Timers
	float StateTimer = 0.0f;

	void TickTracking(float DeltaTime);
	void TickWindUp(float DeltaTime);
	void TickDashing(float DeltaTime);
	void TickRecover(float DeltaTime);
};
