// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
AFirstPersonProjectile::AFirstPersonProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFirstPersonProjectile::OnHit);

	// Set as root component and UpdatedComponent
	RootComponent = CollisionComponent;

	ProjectileMovement->UpdatedComponent = CollisionComponent;

	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.2f;
	ProjectileMovement->Friction = 0.8f;

	// Disappear after 5.0 seconds by default.
	InitialLifeSpan = ProjectileLifespan;

}

// Called when the game starts or when spawned
void AFirstPersonProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFirstPersonProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFirstPersonProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// If we hit the ground (mostly-up surface normal), lay the dart flat.
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