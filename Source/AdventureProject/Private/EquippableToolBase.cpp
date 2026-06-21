// Fill out your copyright notice in the Description page of Project Settings.


#include "EquippableToolBase.h"
#include "AdventureCharacter.h"
#include "InputMappingContext.h"


// Sets default values
AEquippableToolBase::AEquippableToolBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(false);

	// Create this tool's mesh component
	ToolMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ToolMesh"));
	check(ToolMeshComponent != nullptr);
	SetRootComponent(ToolMeshComponent);
}

// Called when the game starts or when spawned
void AEquippableToolBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEquippableToolBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEquippableToolBase::Use()
{
}

void AEquippableToolBase::UseAtTarget(FVector)
{
	Use();
}

void AEquippableToolBase::BindInputAction(const UInputAction* ActionToBind)
{
}

bool AEquippableToolBase::GetServerUseRateLimit(float& OutUseInterval, float& OutCooldownRemaining) const
{
	OutUseInterval = 0.0f;
	OutCooldownRemaining = 0.0f;
	return false;
}
