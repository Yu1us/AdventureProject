// Fill out your copyright notice in the Description page of Project Settings.

#include "DashEnemy.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

namespace
{
constexpr float DebugRefreshLifetime = 0.1f;
}

ADashEnemy::ADashEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Orange tint for dash enemies
	EnemyTint = FLinearColor(1.0f, 0.5f, 0.0f);
}

void ADashEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || IsDead())
	{
		return;
	}

	switch (CurrentState)
	{
	case EDashState::Tracking:
		TickTracking(DeltaTime);
		break;
	case EDashState::WindUp:
		TickWindUp(DeltaTime);
		break;
	case EDashState::Dashing:
		TickDashing(DeltaTime);
		break;
	case EDashState::Recover:
		TickRecover(DeltaTime);
		break;
	}
}

void ADashEnemy::TickTracking(float DeltaTime)
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
	FVector ToPlayer = (PlayerLocation - MyLocation);
	ToPlayer.Z = 0.0f;
	if (!ToPlayer.IsNearlyZero())
	{
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), ToPlayer.Rotation(), DeltaTime, 5.0f));
	}

	// Move toward player slowly while tracking
	AddMovementInput(ToPlayer.GetSafeNormal(), 0.5f);

	// Start windup when in range
	if (Distance < DashRange)
	{
		CurrentState = EDashState::WindUp;
		StateTimer = WindUpTime;

		// Lock dash direction toward player's current position
		DashDirection = (PlayerLocation - MyLocation).GetSafeNormal();
		DashDirection.Z = 0.0f;
	}
}

void ADashEnemy::TickWindUp(float DeltaTime)
{
	// Attack telegraph for the dash path.
	const FVector Start = GetActorLocation();
	const FVector End = Start + DashDirection * DashRange;
	const float Lifetime = FMath::Max(DebugRefreshLifetime, DeltaTime * 2.0f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, Lifetime, 0, 3.0f);
	DrawDebugSphere(GetWorld(), Start + FVector(0, 0, 50), 30.0f, 8, FColor::Red, false, Lifetime);

	StateTimer -= DeltaTime;
	if (StateTimer <= 0.0f)
	{
		CurrentState = EDashState::Dashing;
		DashStartLocation = GetActorLocation();

		// Disable gravity during dash for straight-line movement
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void ADashEnemy::TickDashing(float DeltaTime)
{
	// Calculate movement this frame
	const FVector Delta = DashDirection * DashSpeed * DeltaTime;
	const FVector CurrentLocation = GetActorLocation();
	const FVector NewLocation = CurrentLocation + Delta;

	// Sweep to detect wall hits
	FHitResult Hit;
	SetActorLocation(NewLocation, true, &Hit);

	// Check if we hit something
	if (Hit.bBlockingHit)
	{
		// Hit a wall or other static object - stop and recover
		if (Hit.Component.IsValid() && !Hit.Component->IsSimulatingPhysics())
		{
			CurrentState = EDashState::Recover;
			StateTimer = RecoverTime;
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			return;
		}
	}

	// Check if we hit the player (distance-based)
	APawn* Player = GetTargetPlayer();
	if (Player)
	{
		const float DistToPlayer = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
		if (DistToPlayer < 120.0f)
		{
			DamagePlayer(DashDamage);
			CurrentState = EDashState::Recover;
			StateTimer = RecoverTime;
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			return;
		}
	}

	// Check max dash distance
	const float DashedDistance = FVector::Dist(DashStartLocation, GetActorLocation());
	if (DashedDistance > MaxDashDistance)
	{
		CurrentState = EDashState::Recover;
		StateTimer = RecoverTime;
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

void ADashEnemy::TickRecover(float DeltaTime)
{
	// Stunned - just wait
	const float Lifetime = FMath::Max(DebugRefreshLifetime, DeltaTime * 2.0f);
	DrawDebugSphere(GetWorld(), GetActorLocation() + FVector(0, 0, 80), 20.0f, 8, FColor::Yellow, false, Lifetime);

	StateTimer -= DeltaTime;
	if (StateTimer <= 0.0f)
	{
		CurrentState = EDashState::Tracking;
	}
}
