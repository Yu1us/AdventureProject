// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AdventureCharacter.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	// Use a simple sphere as the collision representation
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	check(CollisionComponent != nullptr);

	// Create this projectile's mesh component
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	check(ProjectileMesh != nullptr);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	check(ProjectileMovement != nullptr);

	// Attach the sphere component to the mesh component
	ProjectileMesh->SetupAttachment(CollisionComponent);

	CollisionComponent->InitSphereRadius(5.0f);

	// Set the collision profile to the "Projectile" collision preset
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");

	// Set up a notification for when this component hits something blocking
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

	// Set as root component and UpdatedComponent
	RootComponent = CollisionComponent;

	ProjectileMovement->UpdatedComponent = CollisionComponent;

	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.4f;
	ProjectileMovement->Friction = 0.7f;
	ProjectileMovement->OnProjectileBounce.AddDynamic(this, &AProjectileBase::OnProjectileBounce);

	// Disappear after 5.0 seconds by default.
	InitialLifeSpan = ProjectileLifespan;

}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	// If we hit a character (enemy), deal damage and destroy the projectile
	if (AAdventureCharacter* HitCharacter = Cast<AAdventureCharacter>(OtherActor))
	{
		if (HitCharacter == GetInstigator())
		{
			return;
		}

		if (!bRequireBounceBeforeDamage || bHasBounced)
		{
			HitCharacter->TakeDamageWrapper(ProjectileDamage, this);
		}

		Destroy();
		return;
	}

	// If we hit the ground (mostly-up surface normal), lay the projectile flat.
	if (FVector::DotProduct(Hit.ImpactNormal, FVector::UpVector) > 0.7f)
	{
		FRotator Flat = GetActorRotation();
		Flat.Pitch = 0.f;
		Flat.Roll = 0.f;
		SetActorRotation(Flat);

		return;
	}

	// Only add impulse and destroy projectile if we hit a physics object
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * PhysicsForce, GetActorLocation());

		Destroy();
	}
}

void AProjectileBase::OnProjectileBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (HasAuthority())
	{
		bHasBounced = true;
	}
}
