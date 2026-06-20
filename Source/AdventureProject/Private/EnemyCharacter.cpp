#include "EnemyCharacter.h"
#include "AdventureGameMode.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

AEnemyCharacter::AEnemyCharacter()
{
	// Enemy uses the default AI controller instead of a player controller
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	// Disable first-person components — enemies don't need them
	FirstPersonCameraComponent->Deactivate();
	FirstPersonCameraComponent->SetVisibility(false);
	FirstPersonCameraComponent->PrimaryComponentTick.bCanEverTick = false;

	FirstPersonMeshComponent->SetVisibility(false);
	FirstPersonMeshComponent->SetComponentTickEnabled(false);
}

void AEnemyCharacter::BeginPlay()
{
	// Skip AAdventureCharacter::BeginPlay entirely — call grandparent directly.
	// This avoids: FP anim setup, owner-see visibility, input mapping context.
	ACharacter::BeginPlay();

	// Re-init health (in case subclass constructor changed MaxHealth)
	CurrentHealth = MaxHealth;

	// Set up the third-person mesh for enemy use
	if (ThirdPersonDefaultAnimClass)
	{
		GetMesh()->SetAnimInstanceClass(ThirdPersonDefaultAnimClass);
	}
	GetMesh()->SetOwnerNoSee(false);
	GetMesh()->CastShadow = true;

	// Apply color tint via dynamic material instance
	if (GetMesh() && GetMesh()->GetMaterial(0))
	{
		UMaterialInstanceDynamic* DynMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
		if (DynMaterial)
		{
			DynMaterial->SetVectorParameterValue(TEXT("Base Color"), EnemyTint);
		}
	}
}

void AEnemyCharacter::OnDeath_Implementation()
{
	if (AAdventureGameMode* GM = Cast<AAdventureGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		GM->RegisterEnemyKill();
	}
	Destroy();
}

APawn* AEnemyCharacter::GetTargetPlayer() const
{
	return UGameplayStatics::GetPlayerPawn(this, 0);
}

void AEnemyCharacter::DamagePlayer(float Amount)
{
	APawn* Player = GetTargetPlayer();
	if (AAdventureCharacter* PlayerChar = Cast<AAdventureCharacter>(Player))
	{
		PlayerChar->TakeDamageWrapper(Amount, this);
	}
}
