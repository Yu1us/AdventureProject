// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventureGameState.h"
#include "Net/UnrealNetwork.h"

AAdventureGameState::AAdventureGameState()
{
	bReplicates = true;
}

void AAdventureGameState::SetMatchState(int32 InKillTarget, int32 InCurrentStreak, bool bInGameEnded, EAdventureMatchResult InMatchResult)
{
	KillTarget = InKillTarget;
	CurrentStreak = InCurrentStreak;
	bGameEnded = bInGameEnded;
	MatchResult = InMatchResult;
}

void AAdventureGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAdventureGameState, KillTarget);
	DOREPLIFETIME(AAdventureGameState, CurrentStreak);
	DOREPLIFETIME(AAdventureGameState, bGameEnded);
	DOREPLIFETIME(AAdventureGameState, MatchResult);
}

void AAdventureGameState::OnRep_KillTarget()
{
	ShowComboDebug();
}

void AAdventureGameState::OnRep_CurrentStreak()
{
	ShowComboDebug();
}

void AAdventureGameState::OnRep_GameEnded()
{
}

void AAdventureGameState::OnRep_MatchResult()
{
	ShowResultDebug();
}

void AAdventureGameState::ShowComboDebug() const
{
	const FString Message = FString::Printf(TEXT("Combo: %d / %d"), CurrentStreak, KillTarget);
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, Message);
	}
}

void AAdventureGameState::ShowResultDebug() const
{
	if (!bGameEnded)
	{
		return;
	}

	FString Message;
	FColor Color = FColor::White;

	switch (MatchResult)
	{
	case EAdventureMatchResult::Victory:
		Message = FString::Printf(TEXT("=== VICTORY! %d kills without getting hit! ==="), KillTarget);
		Color = FColor::Green;
		break;
	case EAdventureMatchResult::Defeat:
		Message = TEXT("=== DEFEAT! Player died. ===");
		Color = FColor::Red;
		break;
	default:
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 30.0f, Color, Message);
	}
}
