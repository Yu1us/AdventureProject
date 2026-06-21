// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREPROJECT_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();
	
	// The amount of force this projectile imparts on objects it collides with
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Physics")
	float PhysicsForce = 100.0f;

	// Damage dealt to characters on hit
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Damage")
	float ProjectileDamage = 25.0f;

	// If enabled, the projectile must bounce before it can damage characters.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Damage")
	bool bRequireBounceBeforeDamage = false;

	// Called when the projectile collides with an object

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Mesh")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	// Despawn after 5 seconds by default
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Lifespan")
	float ProjectileLifespan = 5.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Sphere collision component 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	// Projectile movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Projectile | Damage")
	bool bHasBounced = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
