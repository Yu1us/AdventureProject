#include "AdventureGameMode.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"

void AAdventureGameMode::StartPlay()
{
	Super::StartPlay();

	check(GEngine != nullptr);

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
		FString::Printf(TEXT("Combo Challenge: Kill %d enemies without getting hit!"), KillTarget));
	UE_LOG(LogTemp, Warning, TEXT("AdventureGameMode: KillTarget = %d"), KillTarget);
}

void AAdventureGameMode::RegisterEnemyKill()
{
	if (bGameEnded)
	{
		return;
	}

	++CurrentStreak;

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan,
		FString::Printf(TEXT("Combo: %d / %d"), CurrentStreak, KillTarget));

	if (CurrentStreak >= KillTarget)
	{
		TriggerVictory();
	}
}

void AAdventureGameMode::NotifyPlayerHit()
{
	if (bGameEnded)
	{
		return;
	}

	if (CurrentStreak > 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red,
			FString::Printf(TEXT("Combo broken! (%d reset to 0)"), CurrentStreak));
		CurrentStreak = 0;
	}
}

void AAdventureGameMode::NotifyPlayerDeath()
{
	if (bGameEnded)
	{
		return;
	}

	TriggerDefeat();
}

void AAdventureGameMode::TriggerVictory()
{
	bGameEnded = true;

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Green,
		FString::Printf(TEXT("=== VICTORY! %d kills without getting hit! ==="), KillTarget));
	UE_LOG(LogTemp, Warning, TEXT("VICTORY! %d combo kills achieved."), KillTarget);

	StopAllSpawners();
}

void AAdventureGameMode::TriggerDefeat()
{
	bGameEnded = true;

	GEngine->AddOnScreenDebugMessage(-1, 30.0f, FColor::Red,
		TEXT("=== DEFEAT! Player died. ==="));
	UE_LOG(LogTemp, Warning, TEXT("DEFEAT! Player died."));

	StopAllSpawners();
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