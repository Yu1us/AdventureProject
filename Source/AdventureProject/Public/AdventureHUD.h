// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	void DrawCrosshair();
	void DrawStatusPanel(const AAdventureCharacter* Character, const AAdventureGameState* AdventureGameState, const AAdventurePlayerState* AdventurePlayerState);
	void DrawToolRateLimit(const AAdventureCharacter* Character, float X, float& Y);
	void DrawResultBanner(const AAdventureGameState* AdventureGameState);
	void DrawBar(float X, float Y, float Width, float Height, float Percent, const FLinearColor& FillColor, const FString& Label);
	void DrawLineText(const FString& Text, const FLinearColor& Color, float X, float& Y, float Scale = 1.0f);

	TWeakObjectPtr<const AAdventureCharacter> LastObservedCharacter;
	float LastObservedHealth = -1.0f;
	float DamageFeedbackEndTime = 0.0f;
};
