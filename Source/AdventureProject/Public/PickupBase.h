// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/SphereComponent.h"
#include "AdventureCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class UItemDefinition;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREPROJECT_API APickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupBase();

	// Initializes this pickup with values from the data table.
	void InitializePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The ID of this pickup in the associated data table.
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	FName PickupItemID;

	// Data table that contains this pickup.
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	TSoftObjectPtr<UDataTable> PickupDataTable;

	// Data asset associated with this item.
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Reference Item")
	TObjectPtr<UItemDefinition> ReferenceItem;

	// The mesh component to represent this pickup in the world.
	UPROPERTY(VisibleDefaultsOnly, Category = "Pickup | Mesh")
	TObjectPtr<UStaticMeshComponent> PickupMeshComponent;

	// Sphere Component that defines the collision radius of this pickup for interaction purposes.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Components")
	TObjectPtr<USphereComponent> SphereComponent;

	// Code for when something overlaps the SphereComponent. 
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Whether this pickup should respawn after being picked up.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	bool bShouldRespawn;

	// The time in seconds to wait before respawning this pickup.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	float RespawnTime = 4.0f;

	// Timer handle to distinguish the respawn timer.
	FTimerHandle RespawnTimerHandle;

#if WITH_EDITOR
	// Runs whenever a property on this object is changed in the editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
