// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemySpawner.h"
#include "EnemyCharacter.h"
#include "AdventureGameMode.h"
#include "Kismet/GameplayStatics.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// Simple root component so the spawner has a position in the level
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && bAutoStart)
	{
		StartSpawning();
	}
}

void AEnemySpawner::StartSpawning()
{
	if (!HasAuthority())
	{
		return;
	}

	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AEnemySpawner::SpawnEnemy, SpawnInterval, true, 0.0f);
}

void AEnemySpawner::SpawnEnemy()
{
	if (!HasAuthority() || !EnemyClass)
	{
		return;
	}
	// Stop spawning if the game has ended
	if (AAdventureGameMode* GM = Cast<AAdventureGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		if (GM->bGameEnded)
		{
			StopSpawning();
			return;
		}
	}


	// Clean up dead enemies and check count
	const int32 AliveCount = CleanupAndGetAliveCount();
	if (AliveCount >= MaxEnemies)
	{
		return;
	}

	// Random position within spawn radius
	const FVector Origin = GetActorLocation();
	const float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
	const float Dist = FMath::FRandRange(0.0f, SpawnRadius);
	const FVector SpawnLocation = Origin + FVector(FMath::Cos(Angle) * Dist, FMath::Sin(Angle) * Dist, 0.0f);

	// Spawn with default rotation
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyCharacter* Spawned = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
	if (Spawned)
	{
		AliveEnemies.Add(Spawned);
	}
}

int32 AEnemySpawner::CleanupAndGetAliveCount()
{
	// Remove entries where the enemy has been destroyed
	AliveEnemies.RemoveAll([](const TWeakObjectPtr<AEnemyCharacter>& Ptr)
	{
		return !Ptr.IsValid();
	});

	return AliveEnemies.Num();
}

void AEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}
