// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/DartLauncher.h"
#include "FirstPersonProjectile.h"  
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h" 
#include "AdventureCharacter.h"

void ADartLauncher::Use()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Using the dart launcher!"));

	if (OwningCharacter == nullptr)
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
	if (World != nullptr && ProjectileClass != nullptr)
	{
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
		World->SpawnActor<AFirstPersonProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

	}
}

bool ADartLauncher::ShouldRequestServerUse() const
{
	return OwningCharacter != nullptr && !OwningCharacter->HasAuthority();
}

void ADartLauncher::BindInputAction(const UInputAction* InputToBind)
{
	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(InputToBind, ETriggerEvent::Triggered, this, &ADartLauncher::Use);

		}
	}
}
