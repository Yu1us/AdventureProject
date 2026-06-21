// Fill out your copyright notice in the Description page of Project Settings.

#include "RaycastEnemy.h"
#include "AdventureCharacter.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

namespace
{
constexpr float DebugRefreshLifetime = 0.1f;
constexpr float FireDebugLifetime = 0.5f;
}

ARaycastEnemy::ARaycastEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Purple tint for raycast enemies
	EnemyTint = FLinearColor(0.7f, 0.2f, 1.0f);
}

void ARaycastEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority() || IsDead())
	{
		return;
	}

	switch (CurrentState)
	{
	case ERaycastState::Aiming:
		TickAiming(DeltaTime);
		break;
	case ERaycastState::Charging:
		TickCharging(DeltaTime);
		break;
	case ERaycastState::Firing:
		TickFiring(DeltaTime);
		break;
	case ERaycastState::Cooldown:
		TickCooldown(DeltaTime);
		break;
	}
}

void ARaycastEnemy::TickAiming(float DeltaTime)
{
	APawn* Player = GetTargetPlayer();
	if (!Player)
	{
		return;
	}

	const FVector MyLocation = GetActorLocation();
	const FVector PlayerLocation = Player->GetActorLocation();
	const FVector ToPlayer = PlayerLocation - MyLocation;
	const float Distance = ToPlayer.Size();

	// Only aggro within range
	if (Distance > AggroRange)
	{
		return;
	}

	// Face the player
	FVector FlatToPlayer = ToPlayer;
	FlatToPlayer.Z = 0.0f;
	if (!FlatToPlayer.IsNearlyZero())
	{
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), FlatToPlayer.Rotation(), DeltaTime, 6.0f));
	}

	// Line trace to check if we can see the player
	FHitResult Hit;
	const FVector Start = MyLocation + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector End = Start + ToPlayer.GetSafeNormal() * FMath::Min(Distance, RayRange);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);

	// If trace reaches the player (or nothing blocks), begin charging
	if (!bHit || Hit.GetActor() == Player)
	{
		FireTargetLocation = PlayerLocation;
		CurrentState = ERaycastState::Charging;
		StateTimer = ChargeTime;
	}
}

void ARaycastEnemy::TickCharging(float DeltaTime)
{
	APawn* Player = GetTargetPlayer();
	if (!Player)
	{
		CurrentState = ERaycastState::Aiming;
		return;
	}

	const FVector MyLocation = GetActorLocation();
	const FVector ToTarget = FireTargetLocation - MyLocation;

	// Keep facing the committed attack direction during charge.
	FVector FlatToTarget = ToTarget;
	FlatToTarget.Z = 0.0f;
	if (!FlatToTarget.IsNearlyZero())
	{
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), FlatToTarget.Rotation(), DeltaTime, 10.0f));
	}

	// Attack telegraph while the raycast shot charges.
	const FVector Start = MyLocation + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector Direction = (FireTargetLocation - Start).GetSafeNormal();
	const FVector End = Start + Direction * RayRange;
	const float Lifetime = FMath::Max(DebugRefreshLifetime, DeltaTime * 2.0f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, Lifetime, 0, 3.0f);

	// Countdown
	StateTimer -= DeltaTime;
	if (StateTimer <= 0.0f)
	{
		CurrentState = ERaycastState::Firing;
	}
}

void ARaycastEnemy::TickFiring(float DeltaTime)
{
	// Fire a raycast toward the locked target
	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	const FVector Direction = (FireTargetLocation - Start).GetSafeNormal();
	const FVector End = Start + Direction * RayRange;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, Params);

	DrawDebugLine(GetWorld(), Start, bHit ? Hit.ImpactPoint : End, FColor::Yellow, false, FireDebugLifetime, 0, 2.0f);

	if (bHit)
	{
		if (AAdventureCharacter* HitChar = Cast<AAdventureCharacter>(Hit.GetActor()))
		{
			HitChar->TakeDamageWrapper(RayDamage, this);
		}
	}

	// Enter cooldown
	StateTimer = RayCooldown;
	CurrentState = ERaycastState::Cooldown;
}

void ARaycastEnemy::TickCooldown(float DeltaTime)
{
	StateTimer -= DeltaTime;
	if (StateTimer <= 0.0f)
	{
		CurrentState = ERaycastState::Aiming;
	}
}
