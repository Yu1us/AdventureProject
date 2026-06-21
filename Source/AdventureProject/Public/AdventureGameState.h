// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "AdventureGameState.generated.h"

UENUM(BlueprintType)
enum class EAdventureMatchResult : uint8
{
	InProgress,
	Victory,
	Defeat
};

UCLASS()
class ADVENTUREPROJECT_API AAdventureGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AAdventureGameState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_KillTarget, Category = "Game")
	int32 KillTarget = 10;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_GameEnded, Category = "Game")
	bool bGameEnded = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchResult, Category = "Game")
	EAdventureMatchResult MatchResult = EAdventureMatchResult::InProgress;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_WinnerPlayerName, Category = "Game")
	FString WinnerPlayerName;

	void SetMatchState(int32 InKillTarget, bool bInGameEnded, EAdventureMatchResult InMatchResult, const FString& InWinnerPlayerName);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_KillTarget();

	UFUNCTION()
	void OnRep_GameEnded();

	UFUNCTION()
	void OnRep_MatchResult();

	UFUNCTION()
	void OnRep_WinnerPlayerName();

	void ShowObjectiveDebug() const;
	void ShowResultDebug() const;
	void ShowScoreboardDebug() const;
};
