// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupBase.h"
#include "ItemDefinition.h"
#include "Data/ItemData.h" 

// Sets default values
APickupBase::APickupBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create this pickup's mesh component
	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	check(PickupMeshComponent != nullptr);
	// Make the mesh the root component     
	SetRootComponent(PickupMeshComponent);

	// Create this pickup's collision shape (sphere component)
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	check(SphereComponent != nullptr);

	// Attach the sphere component to the mesh component
	SphereComponent->SetupAttachment(PickupMeshComponent);

	// Set the sphere's collision radius
	SphereComponent->SetSphereRadius(32.f);

	// Configure the sphere to actually generate overlap events
	SphereComponent->SetGenerateOverlapEvents(true);
	// Enable collision    
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// Set the sphere's response to every collision channel to Ignore, removing any engine defaults
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Overrides the previous collision rule for the Pawn channel, so encountering anything on that channel generates an overlap
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();
	
	// Ensure we don't bind the overlap event more than once
	SphereComponent->OnComponentBeginOverlap.RemoveAll(this);

	// Register the overlap event so it runs when an overlap happens
	SphereComponent->OnComponentBeginOverlap.AddDynamic(
		this,
		&APickupBase::OnSphereBeginOverlap
	);

	// Initialize this pickup with default values
	InitializePickup();
}

// Initializes the pickup with values retrieved from the associated data table.
void APickupBase::InitializePickup()
{
	// Only initialize if the pickup has valid inputs 
	const FSoftObjectPath TablePath = PickupDataTable.ToSoftObjectPath();
	if (!TablePath.IsNull() && !PickupItemID.IsNone())
	{
		/* Resolve the table soft reference into a usable data table.
		Use the loaded table if available; otherwise load it now. */
		UDataTable* LoadedDataTable = PickupDataTable.IsValid()
			? PickupDataTable.Get()
			: PickupDataTable.LoadSynchronous();
		// Continue only if the DataTable was successfully loaded 
		if (!LoadedDataTable)
		{
			return;
		}

		// Use the pickup ID to look up and save the corresponding table row 
		const FItemData* ItemDataRow = LoadedDataTable->FindRow<FItemData>(PickupItemID, PickupItemID.ToString());
		// Continue only if the row was found 
		if (!ItemDataRow)
		{
			return;
		}

		/* Resolve the Data Asset referenced by this table row.
		Use the Data Asset if available; otherwise load it now.    */
		UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase.IsValid()
			? ItemDataRow->ItemBase.Get()
			: ItemDataRow->ItemBase.LoadSynchronous();
		// Continue only if the Data Asset was successfully loaded 
		if (!TempItemDefinition)
		{
			return;
		}

		// Create a copy of the item with this class as the owner
		ReferenceItem = TempItemDefinition->CreateItemCopy(this);

		// Resolve the item's world mesh from the item definition.
		// This uses the mesh if it's already in memory, or loads it if it isn't.
		UStaticMesh* LoadedMesh = TempItemDefinition->WorldMesh.IsValid()
			? TempItemDefinition->WorldMesh.Get()
			: TempItemDefinition->WorldMesh.LoadSynchronous();

		// Check the mesh is set and can load before using it
		if (LoadedMesh)
		{
			// Set the pickup's mesh
			PickupMeshComponent->SetStaticMesh(LoadedMesh);
		}

		// Restore visibility after respawning
		PickupMeshComponent->SetVisibility(true);
		// Restore collision after respawning 
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

/**
*	Broadcasts an event when a character overlaps this pickup's SphereComponent. Sets the pickup to invisible and uninteractable, and respawns it after a set time.
*	@param OverlappedComponent - the component that was overlapped.
*	@param OtherActor - the Actor overlapping this component.
*	@param OtherComp - the Actor's component that overlapped this component.
*	@param OtherBodyIndex - the index of the overlapped component.
*	@param bFromSweep - whether the overlap was generated from a sweep.
*	@param SweepResult - contains info about the overlap such as surface normals and faces.
*/
// Sets the pickup to invisible and uninteractable, and respawns it after a set time.
void APickupBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Verbose, TEXT("Attempting a pickup collision"));

	// Checking if it's an AdventureCharacter overlapping
	AAdventureCharacter* Character = Cast<AAdventureCharacter>(OtherActor);
	if (Character != nullptr)
	{
		// Give the item to the character
		Character->GiveItem(ReferenceItem);

		// Set this pickup to be invisible and disable collision
		PickupMeshComponent->SetVisibility(false);
		PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// If the pickup should respawn, wait an fRespawnTime amount of seconds before calling InitializePickup() to respawn it
	if (bShouldRespawn)
	{
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupBase::InitializePickup, RespawnTime, false);
	}
}

/*	Updates this pickup whenever a property is changed.
*	@param PropertyChangedEvent - contains info about the property that was changed. */
#if WITH_EDITOR
void APickupBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Handle parent class property changes
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// If a property was changed, get the name of the changed property. Otherwise use none.
	const FName ChangedPropertyName = PropertyChangedEvent.Property
		? PropertyChangedEvent.Property->GetFName()
		: NAME_None;

	// Verify that the changed property is one that affects item lookup in this class
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase, PickupItemID) ||
		ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase, PickupDataTable))
	{
		InitializePickup();
	}

}
#endif
