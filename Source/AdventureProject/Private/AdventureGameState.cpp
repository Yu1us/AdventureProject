// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventureGameState.h"
#include "AdventurePlayerState.h"
#include "Net/UnrealNetwork.h"

AAdventureGameState::AAdventureGameState()
{
	bReplicates = true;
}

void AAdventureGameState::SetMatchState(int32 InKillTarget, bool bInGameEnded, EAdventureMatchResult InMatchResult, const FString& InWinnerPlayerName)
{
	KillTarget = InKillTarget;
	bGameEnded = bInGameEnded;
	MatchResult = InMatchResult;
	WinnerPlayerName = InWinnerPlayerName;
}

void AAdventureGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAdventureGameState, KillTarget);
	DOREPLIFETIME(AAdventureGameState, bGameEnded);
	DOREPLIFETIME(AAdventureGameState, MatchResult);
	DOREPLIFETIME(AAdventureGameState, WinnerPlayerName);
}

void AAdventureGameState::OnRep_KillTarget()
{
	ShowObjectiveDebug();
}

void AAdventureGameState::OnRep_GameEnded()
{
	ShowResultDebug();
}

void AAdventureGameState::OnRep_MatchResult()
{
	ShowResultDebug();
}

void AAdventureGameState::OnRep_WinnerPlayerName()
{
	ShowResultDebug();
}

void AAdventureGameState::ShowObjectiveDebug() const
{
	const FString Message = FString::Printf(TEXT("Personal NPC kill target: %d"), KillTarget);
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);
}

void AAdventureGameState::ShowResultDebug() const
{
	if (!bGameEnded)
	{
		return;
	}

	FString Message;

	switch (MatchResult)
	{
	case EAdventureMatchResult::Victory:
		Message = WinnerPlayerName.IsEmpty()
			? FString::Printf(TEXT("=== VICTORY! A player reached %d NPC kills! ==="), KillTarget)
			: FString::Printf(TEXT("=== VICTORY! %s reached %d NPC kills! ==="), *WinnerPlayerName, KillTarget);
		break;
	case EAdventureMatchResult::Defeat:
		Message = TEXT("=== DEFEAT! Player died. ===");
		break;
	default:
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	ShowScoreboardDebug();
}

void AAdventureGameState::ShowScoreboardDebug() const
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (const AAdventurePlayerState* AdventurePlayerState = Cast<AAdventurePlayerState>(PlayerState))
		{
			const FString Message = FString::Printf(
				TEXT("%s | PvP:%d NPC:%d D:%d Damage:%.0f NPCCombo:%d BestNPCCombo:%d"),
				*AdventurePlayerState->GetPlayerName(),
				AdventurePlayerState->PlayerKills,
				AdventurePlayerState->NpcKills,
				AdventurePlayerState->Deaths,
				AdventurePlayerState->DamageTaken,
				AdventurePlayerState->CurrentCombo,
				AdventurePlayerState->BestCombo);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
		}
	}
}
