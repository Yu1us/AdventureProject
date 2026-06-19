// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "MeleeChaseEnemy.generated.h"

UENUM(BlueprintType)
enum class EMeleeState : uint8
{
	Chasing,
	Attacking,
	Cooldown
};

/**
 * Simple melee enemy: chases the player, attacks at close range, then cools down.
 */
UCLASS()
class ADVENTUREPROJECT_API AMeleeChaseEnemy : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	AMeleeChaseEnemy();

	// How close the enemy needs to be to attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	float AttackRange = 150.0f;

	// Damage per hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	float MeleeDamage = 10.0f;

	// Seconds between attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	float AttackCooldown = 1.5f;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	EMeleeState CurrentState = EMeleeState::Chasing;
	float CooldownTimer = 0.0f;

	void TickChasing(float DeltaTime);
	void TickAttacking(float DeltaTime);
	void TickCooldown(float DeltaTime);
};
