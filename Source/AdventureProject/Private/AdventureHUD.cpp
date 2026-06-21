// Fill out your copyright notice in the Description page of Project Settings.

#include "AdventureHUD.h"

#include "AdventureCharacter.h"
#include "AdventureGameState.h"
#include "AdventurePlayerState.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"

void AAdventureHUD::DrawHUD()
{
	Super::DrawHUD();

	if (Canvas == nullptr)
	{
		return;
	}

	const APlayerController* OwningController = GetOwningPlayerController();
	const AAdventureCharacter* AdventureCharacter = OwningController ? Cast<AAdventureCharacter>(OwningController->GetPawn()) : nullptr;
	const AAdventurePlayerState* AdventurePlayerState = OwningController ? OwningController->GetPlayerState<AAdventurePlayerState>() : nullptr;
	const AAdventureGameState* AdventureGameState = GetWorld() ? GetWorld()->GetGameState<AAdventureGameState>() : nullptr;

	DrawCrosshair();
	DrawStatusPanel(AdventureCharacter, AdventureGameState, AdventurePlayerState);
	DrawResultBanner(AdventureGameState);
}

void AAdventureHUD::DrawCrosshair()
{
	const float CenterX = Canvas->ClipX * 0.5f;
	const float CenterY = Canvas->ClipY * 0.5f;
	const FLinearColor CrosshairColor(0.95f, 0.95f, 0.9f, 0.9f);
	const float Gap = 5.0f;
	const float Arm = 12.0f;
	const float Thickness = 1.5f;

	DrawLine(CenterX - Gap - Arm, CenterY, CenterX - Gap, CenterY, CrosshairColor, Thickness);
	DrawLine(CenterX + Gap, CenterY, CenterX + Gap + Arm, CenterY, CrosshairColor, Thickness);
	DrawLine(CenterX, CenterY - Gap - Arm, CenterX, CenterY - Gap, CrosshairColor, Thickness);
	DrawLine(CenterX, CenterY + Gap, CenterX, CenterY + Gap + Arm, CrosshairColor, Thickness);
	DrawRect(CrosshairColor, CenterX - 1.0f, CenterY - 1.0f, 2.0f, 2.0f);
}

void AAdventureHUD::DrawStatusPanel(const AAdventureCharacter* Character, const AAdventureGameState* AdventureGameState, const AAdventurePlayerState* AdventurePlayerState)
{
	const float X = 24.0f;
	const float Y = 24.0f;
	const float Width = 440.0f;
	const float Height = 190.0f;
	float CursorY = Y + 14.0f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.45f), X, Y, Width, Height);
	DrawRect(FLinearColor(0.1f, 0.55f, 0.9f, 0.85f), X, Y, 5.0f, Height);

	DrawLineText(TEXT("PvPvE Challenge"), FLinearColor(0.9f, 0.95f, 1.0f, 1.0f), X + 18.0f, CursorY, 1.2f);

	if (Character != nullptr)
	{
		const float HealthPercent = Character->MaxHealth > 0.0f ? FMath::Clamp(Character->CurrentHealth / Character->MaxHealth, 0.0f, 1.0f) : 0.0f;
		const FLinearColor HealthColor = HealthPercent > 0.6f
			? FLinearColor(0.1f, 0.85f, 0.25f, 0.95f)
			: HealthPercent > 0.3f
				? FLinearColor(0.95f, 0.75f, 0.1f, 0.95f)
				: FLinearColor(0.95f, 0.1f, 0.1f, 0.95f);

		DrawBar(X + 18.0f, CursorY + 4.0f, 230.0f, 16.0f, HealthPercent, HealthColor,
			FString::Printf(TEXT("Health %.0f / %.0f"), Character->CurrentHealth, Character->MaxHealth));
		CursorY += 31.0f;

		if (Character->IsDead())
		{
			DrawLineText(TEXT("Status: Down - respawning..."), FLinearColor(1.0f, 0.35f, 0.25f, 1.0f), X + 18.0f, CursorY);
		}
	}
	else
	{
		DrawLineText(TEXT("Health: waiting for pawn"), FLinearColor(0.75f, 0.75f, 0.75f, 1.0f), X + 18.0f, CursorY);
	}

	if (AdventureGameState != nullptr)
	{
		const float ObjectivePercent = AdventureGameState->KillTarget > 0
			? FMath::Clamp(static_cast<float>(AdventureGameState->TeamKills) / static_cast<float>(AdventureGameState->KillTarget), 0.0f, 1.0f)
			: 0.0f;

		DrawBar(X + 18.0f, CursorY + 4.0f, 230.0f, 16.0f, ObjectivePercent, FLinearColor(0.15f, 0.65f, 1.0f, 0.95f),
			FString::Printf(TEXT("Objective %d / %d"), AdventureGameState->TeamKills, AdventureGameState->KillTarget));
		CursorY += 31.0f;

		DrawLineText(
			FString::Printf(TEXT("Team Combo: %d"), AdventureGameState->CurrentStreak),
			FLinearColor(0.75f, 0.95f, 1.0f, 1.0f),
			X + 18.0f,
			CursorY);
	}

	if (AdventurePlayerState != nullptr)
	{
		DrawLineText(
			FString::Printf(
				TEXT("You: K %d  D %d  Damage %.0f  Best Combo %d"),
				AdventurePlayerState->Kills,
				AdventurePlayerState->Deaths,
				AdventurePlayerState->DamageTaken,
				AdventurePlayerState->BestCombo),
			FLinearColor(0.85f, 1.0f, 0.85f, 1.0f),
			X + 18.0f,
			CursorY);
	}
}

void AAdventureHUD::DrawResultBanner(const AAdventureGameState* AdventureGameState)
{
	if (AdventureGameState == nullptr || !AdventureGameState->bGameEnded)
	{
		return;
	}

	FString ResultText;
	FLinearColor ResultColor = FLinearColor::White;

	switch (AdventureGameState->MatchResult)
	{
	case EAdventureMatchResult::Victory:
		ResultText = FString::Printf(TEXT("VICTORY - %d enemy kills reached"), AdventureGameState->KillTarget);
		ResultColor = FLinearColor(0.25f, 1.0f, 0.35f, 1.0f);
		break;
	case EAdventureMatchResult::Defeat:
		ResultText = TEXT("DEFEAT");
		ResultColor = FLinearColor(1.0f, 0.2f, 0.15f, 1.0f);
		break;
	default:
		return;
	}

	const float BannerWidth = 560.0f;
	const float BannerHeight = 72.0f;
	const float BannerX = (Canvas->ClipX - BannerWidth) * 0.5f;
	const float BannerY = Canvas->ClipY * 0.18f;

	DrawRect(FLinearColor(0.0f, 0.0f, 0.0f, 0.65f), BannerX, BannerY, BannerWidth, BannerHeight);
	DrawText(ResultText, ResultColor, BannerX + 28.0f, BannerY + 19.0f, nullptr, 1.6f, false);
}

void AAdventureHUD::DrawBar(float X, float Y, float Width, float Height, float Percent, const FLinearColor& FillColor, const FString& Label)
{
	const float ClampedPercent = FMath::Clamp(Percent, 0.0f, 1.0f);
	DrawRect(FLinearColor(0.08f, 0.08f, 0.08f, 0.85f), X, Y, Width, Height);
	DrawRect(FillColor, X, Y, Width * ClampedPercent, Height);
	DrawText(Label, FLinearColor::White, X + Width + 12.0f, Y - 2.0f, nullptr, 0.86f, false);
}

void AAdventureHUD::DrawLineText(const FString& Text, const FLinearColor& Color, float X, float& Y, float Scale)
{
	DrawText(Text, Color, X, Y, nullptr, Scale, false);
	Y += 22.0f * Scale;
}
