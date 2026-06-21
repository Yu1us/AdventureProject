// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdventureGameState.h"
#include "GameFramework/GameModeBase.h"
#include "AdventureGameMode.generated.h"

class AAdventurePlayerState;

/**
 * Core game rules: PvPvE scoring and win/lose conditions.
 */
UCLASS()
class ADVENTUREPROJECT_API AAdventureGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAdventureGameMode();

	// Target NPC kills for one player to win (configurable in editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 KillTarget = 10;

	// Whether the game has ended (win or lose)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	bool bGameEnded = false;

	// Delay before a defeated player is restored at a PlayerStart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	float RespawnDelay = 3.0f;

	// Called by enemies on death: increment the killer's personal NPC score and combo
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterEnemyKill();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterEnemyKillForController(AController* KillerController);

	// Called when player takes damage: reset that player's personal NPC combo
	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerHit();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerDamaged(AController* DamagedController, float DamageAmount);

	// Called when player dies: count the death and optional PvP kill credit
	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerDeath();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerDeathForController(AController* DeadController, AController* KillerController);

protected:
	virtual void StartPlay() override;

private:
	void SyncGameState(EAdventureMatchResult MatchResult, const FString& WinnerPlayerName = FString());
	void TriggerVictory(AAdventurePlayerState* WinnerState);
	void TriggerDefeat();
	void StopAllSpawners();
	void RespawnPlayer(AController* ControllerToRespawn);
};
