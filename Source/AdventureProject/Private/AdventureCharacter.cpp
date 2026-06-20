// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventureCharacter.h"
#include "AdventureGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "InventoryComponent.h"
#include "EquippableToolDefinition.h"
#include "EquippableToolBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AAdventureCharacter::AAdventureCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create a first-person camera component
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FirstPersonCameraComponent != nullptr);

	// Create a first person mesh component for the owning player.
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	// Create an inventory component for the owning player
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// Attach the first-person mesh to the third-person mesh
	FirstPersonMeshComponent->SetupAttachment(GetMesh());

	// Include the first-person mesh in First Person Rendering so camera FOV and scale settings apply to the mesh  
	FirstPersonMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	// Set the first person mesh to not collide with other objects
	FirstPersonMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	// Treat the 3rd-person mesh as a regular world object
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	// Attach the camera component to the first-person Skeletal Mesh.
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, FName("head"));

	// Position the camera slightly above the eyes and rotate it to behind the player's head
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FirstPersonCameraOffset, FRotator(0.0f, 90.0f, -90.0f));

	// Enable the pawn to control camera rotation.
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Enable first-person rendering and set default FOV and scale values
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = FirstPersonFieldOfView;
	FirstPersonCameraComponent->FirstPersonScale = FirstPersonScale;

	CurrentHealth = MaxHealth;
	bReplicates = true;
}

void AAdventureCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAdventureCharacter, CurrentHealth);
	DOREPLIFETIME(AAdventureCharacter, bIsDead);
}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(GEngine != nullptr);

	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;
		SetDeadState(false);
	}

	// Set the animations on the first person mesh.
	if (FirstPersonDefaultAnimClass)
	{
		FirstPersonMeshComponent->SetAnimInstanceClass(FirstPersonDefaultAnimClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is missing FirstPersonDefaultAnimClass."), *GetNameSafe(this));
	}

	// Set the animations on the third-person mesh.
	if (ThirdPersonDefaultAnimClass)
	{
		GetMesh()->SetAnimInstanceClass(ThirdPersonDefaultAnimClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is missing ThirdPersonDefaultAnimClass."), *GetNameSafe(this));
	}

	// Only the owning player sees the first-person mesh
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// Hide the 3rd-person mesh from the owning player
	GetMesh()->SetOwnerNoSee(true);

	// Make the 3rd-person mesh cast a shadow
	GetMesh()->CastShadow = true;
	GetMesh()->bCastHiddenShadow = true;

	// Get the player controller for this character
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get the enhanced input local player subsystem and add a new input mapping context to it
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FirstPersonContext, 0);
		}
	}

	// Display a debug message for five seconds. 
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using AdventureCharacter."));
}

// Called every frame
void AAdventureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAdventureCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind Movement Actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Move);

		// Bind Jump Actions
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Bind Look Actions
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Look);

	}

}

void AAdventureCharacter::Move(const FInputActionValue& Value)
{
	// 2D Vector of movement values returned from the input action
	const FVector2D MovementValue = Value.Get<FVector2D>();

	// Check if the controller possessing this Actor is valid
	if (Controller)
	{
		// Add left and right movement
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		// Add forward and back movement
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementValue.Y);
	}
}

void AAdventureCharacter::Look(const FInputActionValue& Value)
{
	if (Controller)
	{
		const FVector2D LookAxisValue = Value.Get<FVector2D>();
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

bool AAdventureCharacter::IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition)
{
	// Check that the character does not yet have this particular tool
	for (UEquippableToolDefinition* InventoryItem : InventoryComponent->ToolInventory)
	{
		if (ToolDefinition->ID == InventoryItem->ID)
		{
			return true;
		}
	}
	return false;
}

void AAdventureCharacter::AttachTool(UEquippableToolDefinition* ToolDefinition)
{
	// Only equip this tool if it isn't already owned
	if (!IsToolAlreadyOwned(ToolDefinition))
	{
		// Spawn a new instance of the tool to equip
		AEquippableToolBase* ToolToEquip = GetWorld()->SpawnActor<AEquippableToolBase>(ToolDefinition->ToolAsset, this->GetActorTransform());

		// Attach the tool to the First Person Character
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		// Attach the tool to this character, and then the right hand of their first-person mesh
		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules, FName(TEXT("HandGrip_R")));

		// Set the animations on the character's meshes.
		if (ToolToEquip->FirstPersonToolAnimClass)
		{
			FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnimClass);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is missing FirstPersonToolAnimClass."), *GetNameSafe(ToolToEquip));
		}

		if (ToolToEquip->ThirdPersonToolAnimClass)
		{
			GetMesh()->SetAnimInstanceClass(ToolToEquip->ThirdPersonToolAnimClass);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("%s is missing ThirdPersonToolAnimClass."), *GetNameSafe(ToolToEquip));
		}

		// Add the tool to this character's inventory
		InventoryComponent->ToolInventory.Add(ToolDefinition);
		ToolToEquip->OwningCharacter = this;
		EquippedTool = ToolToEquip;

		// Get the player controller for this character
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(ToolToEquip->ToolMappingContext, 1);
			}
			ToolToEquip->BindInputAction(UseAction);
		}

	}
}

void AAdventureCharacter::ServerUseEquippedTool_Implementation(FVector_NetQuantize TargetPosition)
{
	if (EquippedTool != nullptr && !bIsDead && CurrentHealth > 0.0f)
	{
		EquippedTool->UseAtTarget(TargetPosition);
	}
}

void AAdventureCharacter::GiveItem(UItemDefinition* ItemDefinition)
{
	// Case based on the type of the item
	switch (ItemDefinition->ItemType)
	{
	// If the item is a tool, attempt to cast and attach it to the character
	case EItemType::Tool:
	{
		UEquippableToolDefinition* ToolDefinition = Cast<UEquippableToolDefinition>(ItemDefinition);

		if (ToolDefinition != nullptr)
		{
			AttachTool(ToolDefinition);
		}
		else {

		}
		break;
	}
	case EItemType::Consumable:
	{
		// Not yet implemented
		break;
	}
	default:
		break;
	}
}

FVector AAdventureCharacter::GetCameraTargetLocation()
{
	// The target position to return
	FVector TargetPosition = FVector::ZeroVector;

	UWorld* const World = GetWorld();

	if (World != nullptr)
	{
		// The result of the line trace
		FHitResult Hit;

		// Simulate a line trace from the character along the vector they're looking down
		const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
		const FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector() * 10000.0;

		// Simulate a line trace and save result in Hit
		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		// If the trace made a blocking hit, use the impact point as the target. Otherwise, use the end point of the trace.
		TargetPosition = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
	}

	return TargetPosition;
}

float AAdventureCharacter::TakeDamageWrapper(float DamageAmount, AActor* DamageCauser)
{
	if (!HasAuthority() || bIsDead || CurrentHealth <= 0.f || DamageAmount <= 0.f)
	{
		return 0.f;
	}

	LastDamageInstigatorController = ResolveDamageInstigatorController(DamageCauser);

	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Max(0.f, CurrentHealth - DamageAmount);
	const float ActualDamage = PreviousHealth - CurrentHealth;
	ShowHealthDebug();

	// Only notify GameMode for player character (not enemies)
	if (ActualDamage > 0.f && IsPlayerControlled())
	{
		if (AAdventureGameMode* GM = Cast<AAdventureGameMode>(UGameplayStatics::GetGameMode(this)))
		{
			GM->NotifyPlayerDamaged(GetController(), ActualDamage);
		}
	}

	if (CurrentHealth <= 0.f)
	{
		SetDeadState(true);

		if (IsPlayerControlled())
		{
			if (AAdventureGameMode* GM = Cast<AAdventureGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				GM->NotifyPlayerDeathForController(GetController(), LastDamageInstigatorController);
			}
		}
		OnDeath();
	}

	return ActualDamage;
}

void AAdventureCharacter::OnDeath_Implementation()
{
	// Default: do nothing. EnemyCharacter overrides this to Destroy().
}

void AAdventureCharacter::OnRep_CurrentHealth()
{
	ShowHealthDebug();
}

void AAdventureCharacter::RestoreFullHealth()
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentHealth = MaxHealth;
	SetDeadState(false);
	ShowHealthDebug();
}

void AAdventureCharacter::OnRep_IsDead()
{
	ApplyDeadState();
}

AController* AAdventureCharacter::ResolveDamageInstigatorController(AActor* DamageCauser) const
{
	if (DamageCauser == nullptr)
	{
		return nullptr;
	}

	if (AController* InstigatorController = DamageCauser->GetInstigatorController())
	{
		return InstigatorController;
	}

	if (const APawn* InstigatorPawn = Cast<APawn>(DamageCauser->GetInstigator()))
	{
		return InstigatorPawn->GetController();
	}

	if (const APawn* DamagePawn = Cast<APawn>(DamageCauser))
	{
		return DamagePawn->GetController();
	}

	if (const AActor* DamageOwner = DamageCauser->GetOwner())
	{
		return DamageOwner->GetInstigatorController();
	}

	return nullptr;
}

void AAdventureCharacter::SetDeadState(bool bNewIsDead)
{
	bIsDead = bNewIsDead;
	ApplyDeadState();
}

void AAdventureCharacter::ApplyDeadState()
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		if (bIsDead)
		{
			MovementComponent->StopMovementImmediately();
			MovementComponent->DisableMovement();
		}
		else
		{
			MovementComponent->SetMovementMode(MOVE_Walking);
		}
	}

	SetActorEnableCollision(!bIsDead);
	SetActorHiddenInGame(bIsDead);
}

void AAdventureCharacter::ShowHealthDebug() const
{
	const FString Message = FString::Printf(TEXT("%s Health: %.0f / %.0f"), *GetNameSafe(this), CurrentHealth, MaxHealth);
	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, Message);
	}
}
