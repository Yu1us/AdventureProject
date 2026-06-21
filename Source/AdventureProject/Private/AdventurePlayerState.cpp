// Fill out your copyright notice in the Description page of Project Settings.

#include "AdventurePlayerState.h"
#include "Net/UnrealNetwork.h"

AAdventurePlayerState::AAdventurePlayerState()
{
	bReplicates = true;
}

void AAdventurePlayerState::AddPlayerKill()
{
	++PlayerKills;
	ShowStatsDebug();
}

void AAdventurePlayerState::AddNpcKill()
{
	++NpcKills;
	++CurrentCombo;
	BestCombo = FMath::Max(BestCombo, CurrentCombo);
	ShowStatsDebug();
}

void AAdventurePlayerState::AddDeath()
{
	++Deaths;
	ResetCombo();
	ShowStatsDebug();
}

void AAdventurePlayerState::AddDamageTaken(float DamageAmount)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	DamageTaken += DamageAmount;
	ResetCombo();
	ShowStatsDebug();
}

void AAdventurePlayerState::ResetCombo()
{
	CurrentCombo = 0;
}

void AAdventurePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAdventurePlayerState, PlayerKills);
	DOREPLIFETIME(AAdventurePlayerState, NpcKills);
	DOREPLIFETIME(AAdventurePlayerState, Deaths);
	DOREPLIFETIME(AAdventurePlayerState, DamageTaken);
	DOREPLIFETIME(AAdventurePlayerState, CurrentCombo);
	DOREPLIFETIME(AAdventurePlayerState, BestCombo);
}

void AAdventurePlayerState::OnRep_MatchStats()
{
	ShowStatsDebug();
}

void AAdventurePlayerState::ShowStatsDebug() const
{
	const FString Message = FString::Printf(
		TEXT("%s | PvP:%d NPC:%d D:%d Damage:%.0f NPCCombo:%d Best:%d"),
		*GetPlayerName(),
		PlayerKills,
		NpcKills,
		Deaths,
		DamageTaken,
		CurrentCombo,
		BestCombo);

	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
}
