// Fill out your copyright notice in the Description page of Project Settings.

#include "AdventurePlayerState.h"
#include "Net/UnrealNetwork.h"

AAdventurePlayerState::AAdventurePlayerState()
{
	bReplicates = true;
}

void AAdventurePlayerState::AddKill()
{
	++Kills;
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

	DOREPLIFETIME(AAdventurePlayerState, Kills);
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
		TEXT("%s | K:%d D:%d Damage:%.0f Combo:%d Best:%d"),
		*GetPlayerName(),
		Kills,
		Deaths,
		DamageTaken,
		CurrentCombo,
		BestCombo);

	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
}
