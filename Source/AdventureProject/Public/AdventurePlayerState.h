// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AdventurePlayerState.generated.h"

UCLASS()
class ADVENTUREPROJECT_API AAdventurePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AAdventurePlayerState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchStats, Category = "Match Stats")
	int32 PlayerKills = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchStats, Category = "Match Stats")
	int32 NpcKills = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchStats, Category = "Match Stats")
	int32 Deaths = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchStats, Category = "Match Stats")
	float DamageTaken = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchStats, Category = "Match Stats")
	int32 CurrentCombo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_MatchStats, Category = "Match Stats")
	int32 BestCombo = 0;

	void AddPlayerKill();
	void AddNpcKill();
	void AddDeath();
	void AddDamageTaken(float DamageAmount);
	void ResetCombo();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_MatchStats();

	void ShowStatsDebug() const;
};
