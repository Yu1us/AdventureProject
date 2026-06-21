// Fill out your copyright notice in the Description page of Project Settings.

#include "AdventureHUD.h"

#include "AdventureCharacter.h"
#include "EquippableToolBase.h"
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

	UpdateDamageFeedback(AdventureCharacter);

	DrawCrosshair();
	DrawStatusPanel(AdventureCharacter, AdventureGameState, AdventurePlayerState);
	DrawResultBanner(AdventureGameState);
	DrawDamageFeedback();
}

void AAdventureHUD::UpdateDamageFeedback(const AAdventureCharacter* Character)
{
	if (Character == nullptr)
	{
		LastObservedCharacter.Reset();
		LastObservedHealth = -1.0f;
		return;
	}

	if (LastObservedCharacter.Get() != Character)
	{
		LastObservedCharacter = Character;
		LastObservedHealth = Character->CurrentHealth;
		return;
	}

	if (LastObservedHealth >= 0.0f && Character->CurrentHealth < LastObservedHealth - KINDA_SMALL_NUMBER)
	{
		if (const UWorld* World = GetWorld())
		{
			DamageFeedbackEndTime = World->GetTimeSeconds() + 0.35f;
		}
	}

	LastObservedHealth = Character->CurrentHealth;
}

void AAdventureHUD::DrawDamageFeedback()
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	constexpr float FeedbackDuration = 0.35f;
	const float RemainingTime = DamageFeedbackEndTime - World->GetTimeSeconds();
	if (RemainingTime <= 0.0f)
	{
		return;
	}

	const float Alpha = FMath::Clamp(RemainingTime / FeedbackDuration, 0.0f, 1.0f);
	DrawRect(FLinearColor(0.85f, 0.02f, 0.0f, 0.22f * Alpha), 0.0f, 0.0f, Canvas->ClipX, Canvas->ClipY);
}

void AAdventureHUD::DrawCrosshair()
{
	const float CenterX = Canvas->ClipX * 0.5f;
	const float CenterY = Canvas->ClipY * 0.5f;
	const FLinearColor ShadowColor(0.0f, 0.0f, 0.0f, 0.65f);
	const FLinearColor CrosshairColor(0.98f, 0.96f, 0.86f, 0.95f);
	const float Radius = 3.5f;

	for (int32 OffsetY = -4; OffsetY <= 4; ++OffsetY)
	{
		const float HalfWidth = FMath::Sqrt(FMath::Max(0.0f, (Radius + 1.0f) * (Radius + 1.0f) - static_cast<float>(OffsetY * OffsetY)));
		DrawLine(CenterX - HalfWidth, CenterY + OffsetY, CenterX + HalfWidth, CenterY + OffsetY, ShadowColor, 1.0f);
	}

	for (int32 OffsetY = -3; OffsetY <= 3; ++OffsetY)
	{
		const float HalfWidth = FMath::Sqrt(FMath::Max(0.0f, Radius * Radius - static_cast<float>(OffsetY * OffsetY)));
		DrawLine(CenterX - HalfWidth, CenterY + OffsetY, CenterX + HalfWidth, CenterY + OffsetY, CrosshairColor, 1.0f);
	}
}

void AAdventureHUD::DrawStatusPanel(const AAdventureCharacter* Character, const AAdventureGameState* AdventureGameState, const AAdventurePlayerState* AdventurePlayerState)
{
	const float X = 24.0f;
	const float Y = 24.0f;
	const float Width = 440.0f;
	const float Height = 224.0f;
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

		DrawToolRateLimit(Character, X + 18.0f, CursorY);
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

void AAdventureHUD::DrawToolRateLimit(const AAdventureCharacter* Character, float X, float& Y)
{
	const AEquippableToolBase* EquippedTool = Character != nullptr ? Character->GetEquippedTool() : nullptr;
	if (EquippedTool == nullptr)
	{
		DrawLineText(TEXT("Server Fire Limit: no tool"), FLinearColor(0.7f, 0.72f, 0.75f, 1.0f), X, Y, 0.88f);
		return;
	}

	float FireInterval = 0.0f;
	float CooldownRemaining = 0.0f;
	if (!EquippedTool->GetServerUseRateLimit(FireInterval, CooldownRemaining))
	{
		DrawLineText(TEXT("Server Fire Limit: none"), FLinearColor(0.7f, 0.72f, 0.75f, 1.0f), X, Y, 0.88f);
		return;
	}

	const float ShotsPerSecond = FireInterval > 0.0f ? 1.0f / FireInterval : 0.0f;
	DrawLineText(
		FString::Printf(TEXT("Server Fire Limit: %.1f/s  Cooldown: %.2fs"), ShotsPerSecond, CooldownRemaining),
		CooldownRemaining > 0.0f ? FLinearColor(1.0f, 0.82f, 0.35f, 1.0f) : FLinearColor(0.75f, 1.0f, 0.75f, 1.0f),
		X,
		Y,
		0.88f);
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
