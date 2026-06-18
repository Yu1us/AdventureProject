// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonProjectile.h"

// Sets default values
AFirstPersonProjectile::AFirstPersonProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

