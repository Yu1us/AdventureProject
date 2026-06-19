// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

class AEnemyCharacter;

/**
 * Spawns enemies at regular intervals. Place in level to create a spawn point.
 * Tracks alive enemies and respects the max count.
 */
UCLASS()
class ADVENTUREPROJECT_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	AEnemySpawner();

	// Which enemy class to spawn (set to AMeleeChaseEnemy, ADashEnemy, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	// Seconds between spawns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnInterval = 3.0f;

	// Max enemies alive from this spawner at once
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	int32 MaxEnemies = 5;

	// Spawn radius around the spawner
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	float SpawnRadius = 200.0f;

	// Start spawning automatically on BeginPlay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	bool bAutoStart = true;

protected:
	virtual void BeginPlay() override;

	// Call to spawn one enemy (respects MaxEnemies)
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SpawnEnemy();

private:
	FTimerHandle SpawnTimerHandle;

	// Weak references to alive enemies (auto-null when destroyed)
	UPROPERTY()
	TArray<TWeakObjectPtr<AEnemyCharacter>> AliveEnemies;

	// Clean up dead/destroyed entries and return current alive count
	int32 CleanupAndGetAliveCount();

	void StartSpawning();
};
