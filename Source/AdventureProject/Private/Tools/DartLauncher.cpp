// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/DartLauncher.h"
#include "ProjectileBase.h"  
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h" 
#include "AdventureCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

void ADartLauncher::Use()
{
	if (OwningCharacter == nullptr || OwningCharacter->IsDead())
	{
		return;
	}

	const FVector TargetPosition = OwningCharacter->GetCameraTargetLocation();

	if (ShouldRequestServerUse())
	{
		OwningCharacter->ServerUseEquippedTool(FVector_NetQuantize(TargetPosition));
		return;
	}

	UseAtTarget(TargetPosition);
}

void ADartLauncher::UseAtTarget(FVector TargetPosition)
{
	if (OwningCharacter == nullptr || !OwningCharacter->HasAuthority())
	{
		return;
	}

	UWorld* const World = GetWorld();
	if (World == nullptr || ProjectileClass == nullptr)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();
	if (Now < NextServerFireTime)
	{
		return;
	}

	if (ToolMeshComponent == nullptr)
	{
		return;
	}

	NextServerFireTime = Now + FireInterval;

	// Get the correct socket to spawn the projectile from
	FVector SocketLocation = ToolMeshComponent->GetSocketLocation("Muzzle");
	// Get projectile's rotation as it spawns so we know in what direction to apply an offset
	FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, TargetPosition);
	FVector SpawnLocation = SocketLocation + UKismetMathLibrary::GetForwardVector(SpawnRotation) * 10.0;

	//Set Spawn Collision Handling Override
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = OwningCharacter;
	ActorSpawnParams.Instigator = OwningCharacter;
	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	// Spawn the projectile at the muzzle
	World->SpawnActor<AProjectileBase>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
}

bool ADartLauncher::ShouldRequestServerUse() const
{
	return OwningCharacter != nullptr && !OwningCharacter->HasAuthority();
}

void ADartLauncher::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADartLauncher, FireInterval);
	DOREPLIFETIME(ADartLauncher, NextServerFireTime);
}

bool ADartLauncher::GetServerUseRateLimit(float& OutUseInterval, float& OutCooldownRemaining) const
{
	OutUseInterval = FMath::Max(0.0f, FireInterval);
	OutCooldownRemaining = 0.0f;

	const UWorld* World = GetWorld();
	if (World != nullptr)
	{
		const AGameStateBase* GameState = World->GetGameState();
		const float ServerNow = GameState != nullptr ? GameState->GetServerWorldTimeSeconds() : World->GetTimeSeconds();
		OutCooldownRemaining = FMath::Max(0.0f, NextServerFireTime - ServerNow);
	}

	return OutUseInterval > 0.0f;
}

void ADartLauncher::BindInputAction(const UInputAction* InputToBind)
{
	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(InputToBind, ETriggerEvent::Started, this, &ADartLauncher::Use);

		}
	}
}
