// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdventureGameState.h"
#include "GameFramework/GameModeBase.h"
#include "AdventureGameMode.generated.h"

/**
 * Core game rules: scoring (combo streak) and win/lose conditions.
 */
UCLASS()
class ADVENTUREPROJECT_API AAdventureGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAdventureGameMode();

	// Target combo kills to win (configurable in editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 KillTarget = 10;

	// Current combo kill streak (resets on player hit)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 CurrentStreak = 0;

	// Total team kills across all connected players
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 TeamKills = 0;

	// Whether the game has ended (win or lose)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	bool bGameEnded = false;

	// Delay before a defeated player is restored at a PlayerStart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	float RespawnDelay = 3.0f;

	// Called by enemies on death: increment combo
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterEnemyKill();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterEnemyKillForController(AController* KillerController);

	// Called when player takes damage: reset combo
	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerHit();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerDamaged(AController* DamagedController, float DamageAmount);

	// Called when player dies: trigger defeat
	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerDeath();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerDeathForController(AController* DeadController, AController* KillerController);

protected:
	virtual void StartPlay() override;

private:
	void SyncGameState(EAdventureMatchResult MatchResult);
	void TriggerVictory();
	void TriggerDefeat();
	void StopAllSpawners();
	void RespawnPlayer(AController* ControllerToRespawn);
};
