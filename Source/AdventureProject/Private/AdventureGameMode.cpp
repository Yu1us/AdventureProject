#include "AdventureGameMode.h"
#include "AdventureCharacter.h"
#include "EnemySpawner.h"
#include "AdventureHUD.h"
#include "AdventurePlayerState.h"
#include "Kismet/GameplayStatics.h"

AAdventureGameMode::AAdventureGameMode()
{
	GameStateClass = AAdventureGameState::StaticClass();
	PlayerStateClass = AAdventurePlayerState::StaticClass();
	HUDClass = AAdventureHUD::StaticClass();
}

void AAdventureGameMode::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
		FString::Printf(TEXT("PvPvE Challenge: Reach %d enemy kills while fighting players!"), KillTarget));
	UE_LOG(LogTemp, Warning, TEXT("AdventureGameMode: KillTarget = %d"), KillTarget);

	SyncGameState(EAdventureMatchResult::InProgress);
}

void AAdventureGameMode::RegisterEnemyKill()
{
	RegisterEnemyKillForController(nullptr);
}

void AAdventureGameMode::RegisterEnemyKillForController(AController* KillerController)
{
	if (bGameEnded)
	{
		return;
	}

	++CurrentStreak;
	++TeamKills;

	if (AAdventurePlayerState* KillerState = KillerController ? KillerController->GetPlayerState<AAdventurePlayerState>() : nullptr)
	{
		KillerState->AddKill();
	}

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
		FString::Printf(TEXT("Team Kills: %d / %d | Team Combo: %d"), TeamKills, KillTarget, CurrentStreak));

	if (TeamKills >= KillTarget)
	{
		TriggerVictory();
		return;
	}

	SyncGameState(EAdventureMatchResult::InProgress);
}

void AAdventureGameMode::NotifyPlayerHit()
{
	NotifyPlayerDamaged(nullptr, 0.0f);
}

void AAdventureGameMode::NotifyPlayerDamaged(AController* DamagedController, float DamageAmount)
{
	if (bGameEnded)
	{
		return;
	}

	if (AAdventurePlayerState* DamagedState = DamagedController ? DamagedController->GetPlayerState<AAdventurePlayerState>() : nullptr)
	{
		DamagedState->AddDamageTaken(DamageAmount);
	}

	if (CurrentStreak > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
			FString::Printf(TEXT("Combo broken! (%d reset to 0)"), CurrentStreak));
		CurrentStreak = 0;
		SyncGameState(EAdventureMatchResult::InProgress);
	}
}

void AAdventureGameMode::NotifyPlayerDeath()
{
	NotifyPlayerDeathForController(nullptr, nullptr);
}

void AAdventureGameMode::NotifyPlayerDeathForController(AController* DeadController, AController* KillerController)
{
	if (bGameEnded)
	{
		return;
	}

	if (AAdventurePlayerState* DeadState = DeadController ? DeadController->GetPlayerState<AAdventurePlayerState>() : nullptr)
	{
		DeadState->AddDeath();
	}

	if (KillerController != nullptr && KillerController != DeadController)
	{
		if (AAdventurePlayerState* KillerState = KillerController->GetPlayerState<AAdventurePlayerState>())
		{
			KillerState->AddKill();
			const FString DeadPlayerName = DeadController && DeadController->PlayerState ? DeadController->PlayerState->GetPlayerName() : TEXT("Player");

			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange,
				FString::Printf(TEXT("PvP Elimination: %s defeated %s"),
					*KillerState->GetPlayerName(),
					*DeadPlayerName));
		}
	}

	if (DeadController)
	{
		FTimerDelegate RespawnDelegate;
		RespawnDelegate.BindUObject(this, &AAdventureGameMode::RespawnPlayer, DeadController);

		FTimerHandle RespawnTimerHandle;
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
	}

	SyncGameState(EAdventureMatchResult::InProgress);
}

void AAdventureGameMode::TriggerVictory()
{
	bGameEnded = true;
	SyncGameState(EAdventureMatchResult::Victory);

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Green,
		FString::Printf(TEXT("=== VICTORY! Team reached %d kills! ==="), KillTarget));
	UE_LOG(LogTemp, Warning, TEXT("VICTORY! Team reached %d kills."), KillTarget);

	StopAllSpawners();
}

void AAdventureGameMode::TriggerDefeat()
{
	bGameEnded = true;
	SyncGameState(EAdventureMatchResult::Defeat);

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red,
		TEXT("=== DEFEAT! Player died. ==="));
	UE_LOG(LogTemp, Warning, TEXT("DEFEAT! Player died."));

	StopAllSpawners();
}

void AAdventureGameMode::SyncGameState(EAdventureMatchResult MatchResult)
{
	if (AAdventureGameState* AdventureGameState = GetGameState<AAdventureGameState>())
	{
		AdventureGameState->SetMatchState(KillTarget, CurrentStreak, TeamKills, bGameEnded, MatchResult);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AdventureGameMode could not sync match state because GameState is not AAdventureGameState."));
	}
}

void AAdventureGameMode::StopAllSpawners()
{
	TArray<AActor*> SpawnerActors;
	UGameplayStatics::GetAllActorsOfClass(this, AEnemySpawner::StaticClass(), SpawnerActors);

	for (AActor* Actor : SpawnerActors)
	{
		if (AEnemySpawner* Spawner = Cast<AEnemySpawner>(Actor))
		{
			Spawner->StopSpawning();
		}
	}
}

void AAdventureGameMode::RespawnPlayer(AController* ControllerToRespawn)
{
	if (!IsValid(ControllerToRespawn) || bGameEnded)
	{
		return;
	}

	if (AAdventureCharacter* Character = Cast<AAdventureCharacter>(ControllerToRespawn->GetPawn()))
	{
		if (AActor* StartSpot = ChoosePlayerStart(ControllerToRespawn))
		{
			Character->SetActorLocationAndRotation(StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), false, nullptr, ETeleportType::TeleportPhysics);
		}

		Character->RestoreFullHealth();
	}
}
