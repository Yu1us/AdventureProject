// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	// Target combo kills to win (configurable in editor)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int32 KillTarget = 10;

	// Current combo kill streak (resets on player hit)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 CurrentStreak = 0;

	// Whether the game has ended (win or lose)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	bool bGameEnded = false;

	// Called by enemies on death: increment combo
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterEnemyKill();

	// Called when player takes damage: reset combo
	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerHit();

	// Called when player dies: trigger defeat
	UFUNCTION(BlueprintCallable, Category = "Game")
	void NotifyPlayerDeath();

protected:
	virtual void StartPlay() override;

private:
	void TriggerVictory();
	void TriggerDefeat();
	void StopAllSpawners();
};
