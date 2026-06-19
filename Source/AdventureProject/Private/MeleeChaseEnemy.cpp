#include "MeleeChaseEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AMeleeChaseEnemy::AMeleeChaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AMeleeChaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
	case EMeleeState::Chasing:
		TickChasing(DeltaTime);
		break;
	case EMeleeState::Attacking:
		TickAttacking(DeltaTime);
		break;
	case EMeleeState::Cooldown:
		TickCooldown(DeltaTime);
		break;
	}
}

void AMeleeChaseEnemy::TickChasing(float DeltaTime)
{
	APawn* Player = GetTargetPlayer();
	if (!Player)
	{
		return;
	}

	const FVector MyLocation = GetActorLocation();
	const FVector PlayerLocation = Player->GetActorLocation();
	const float Distance = FVector::Dist(MyLocation, PlayerLocation);

	// Face the player
	FVector Direction = (PlayerLocation - MyLocation);
	Direction.Z = 0.0f;
	if (!Direction.IsNearlyZero())
	{
		const FRotator TargetRotation = Direction.Rotation();
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 10.0f));
	}

	// Move toward player
	if (Distance > AttackRange)
	{
		AddMovementInput(Direction.GetSafeNormal(), 1.0f);
	}
	else
	{
		// Close enough — switch to attack
		CurrentState = EMeleeState::Attacking;
	}
}

void AMeleeChaseEnemy::TickAttacking(float DeltaTime)
{
	// Deal damage immediately on entering attack state
	DamagePlayer(MeleeDamage);

	// Switch to cooldown
	CooldownTimer = AttackCooldown;
	CurrentState = EMeleeState::Cooldown;
}

void AMeleeChaseEnemy::TickCooldown(float DeltaTime)
{
	CooldownTimer -= DeltaTime;
	if (CooldownTimer <= 0.0f)
	{
		CurrentState = EMeleeState::Chasing;
	}
}
