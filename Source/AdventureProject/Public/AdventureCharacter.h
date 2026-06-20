// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Engine/NetSerialization.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "InputActionValue.h"
#include "AdventureCharacter.generated.h"

class UAnimInstance;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
class UItemDefinition;
class AEquippableToolBase;
class AController;
class UEquippableToolDefinition;
class UInventoryComponent;

UCLASS()
class ADVENTUREPROJECT_API AAdventureCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdventureCharacter();

	// First Person animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TSubclassOf<UAnimInstance> FirstPersonDefaultAnimClass;

	// Third Person animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	TSubclassOf<UAnimInstance> ThirdPersonDefaultAnimClass;

	// Returns the location in the world the character is looking at
	UFUNCTION()
	FVector GetCameraTargetLocation();
	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentHealth, Category = "Health")
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsDead, Category = "Health")
	bool bIsDead = false;

	// Take damage entry point. Returns actual damage dealt.
	UFUNCTION(BlueprintCallable, Category = "Health")
	virtual float TakeDamageWrapper(float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void RestoreFullHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

	AController* GetLastDamageInstigatorController() const { return LastDamageInstigatorController; }

	// Death event - override in Blueprint or C++ subclass for custom death behavior
	UFUNCTION(BlueprintNativeEvent, Category = "Health")
	void OnDeath();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> FirstPersonContext;

	// Move Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	// Jump Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	// Look Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	// The currently-equipped tool
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tools")
	TObjectPtr<AEquippableToolBase> EquippedTool;

	// Use Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> UseAction;

public:	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles 2D Movement Input
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// Handles Look Input
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	// First Person camera
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	// Offset for the first-person camera
	UPROPERTY(EditAnywhere, Category = Camera)
	FVector FirstPersonCameraOffset = FVector(2.8f, 5.9f, 0.0f);

	// First-person primitives field of view
	UPROPERTY(EditAnywhere, Category = Camera)
	float FirstPersonFieldOfView = 70.0f;

	// First-person primitives view scale
	UPROPERTY(EditAnywhere, Category = Camera)
	float FirstPersonScale = 0.6f;

	// First-person mesh, visible only to the owning player
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	// Inventory Component
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// Returns whether or not the player already owns this tool
	UFUNCTION()
	bool IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition);

	// Attaches and equips a tool to the player
	UFUNCTION()
	void AttachTool(UEquippableToolDefinition* ToolDefinition);

	// Requests the server to use the currently-equipped tool.
	UFUNCTION(Server, Reliable)
	void ServerUseEquippedTool(FVector_NetQuantize TargetPosition);

	// Public function that other classes can call to attempt to give an item to the player
	UFUNCTION()
	void GiveItem(UItemDefinition* ItemDefinition);

private:
	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_IsDead();

	UPROPERTY()
	TObjectPtr<AController> LastDamageInstigatorController;

	AController* ResolveDamageInstigatorController(AActor* DamageCauser) const;
	void SetDeadState(bool bNewIsDead);
	void ApplyDeadState();
	void ShowHealthDebug() const;
};
