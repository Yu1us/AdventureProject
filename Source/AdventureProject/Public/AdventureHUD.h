// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdventureGameState.h"
#include "GameFramework/HUD.h"
#include "AdventureHUD.generated.h"

class AAdventureCharacter;
class AAdventureGameState;
class AAdventurePlayerState;

/**
 * Lightweight C++ HUD for the submission demo.
 * It renders existing replicated health, match, and player-stat state without requiring UMG assets.
 */
UCLASS()
class ADVENTUREPROJECT_API AAdventureHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void DrawHUD() override;

private:
	void UpdateDamageFeedback(const AAdventureCharacter* Character);
	void DrawDamageFeedback();
	void UpdateResultBanner(const AAdventureGameState* AdventureGameState);
	void UpdatePlayerKillMessage(const AAdventurePlayerState* AdventurePlayerState);
	void DrawCrosshair();
	void DrawStatusPanel(const AAdventureCharacter* Character, const AAdventureGameState* AdventureGameState, const AAdventurePlayerState* AdventurePlayerState);
	void DrawToolRateLimit(const AAdventureCharacter* Character, float X, float& Y);
	void DrawResultBanner(const AAdventureGameState* AdventureGameState);
	void DrawPlayerKillMessage();
	void DrawBar(float X, float Y, float Width, float Height, float Percent, const FLinearColor& FillColor, const FString& Label);
	void DrawLineText(const FString& Text, const FLinearColor& Color, float X, float& Y, float Scale = 1.0f);

	TWeakObjectPtr<const AAdventureCharacter> LastObservedCharacter;
	TWeakObjectPtr<const AAdventurePlayerState> LastObservedPlayerState;

	float LastObservedHealth = -1.0f;
	float DamageFeedbackEndTime = 0.0f;

	bool bObservedGameEnded = false;
	EAdventureMatchResult LastObservedMatchResult = EAdventureMatchResult::InProgress;
	int32 LastObservedKillTarget = 0;
	FString LastObservedWinnerPlayerName;
	float ResultBannerEndTime = 0.0f;

	int32 LastObservedPlayerKills = INDEX_NONE;
	int32 LastObservedNpcKills = INDEX_NONE;
	FString PlayerKillMessage;
	float PlayerKillMessageEndTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "HUD|Messages", meta = (ClampMin = "0.0"))
	float ResultBannerDisplayDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "HUD|Messages", meta = (ClampMin = "0.0"))
	float PlayerKillMessageDisplayDuration = 3.0f;
};
