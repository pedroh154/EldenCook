#include "Worktops/EC_ItemSpawner.h"
#include "Items/EC_Item.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EldenCookCharacter.h"
#include "Components/BoxComponent.h"

AEC_ItemSpawner::AEC_ItemSpawner()
{
	ItemSpawnCooldown = 0;
}

void AEC_ItemSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEC_ItemSpawner, CurrentSpawnedItem);
}

void AEC_ItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	//if multiplayer, only start the timer server-side
	if(GetLocalRole() == ROLE_Authority)
	{
		SpawnItem();
	}
}

void AEC_ItemSpawner::SpawnItem()
{
	//do it only server side, item will replicate to clients
	if(GetLocalRole() == ROLE_Authority)
	{
		if(IsValid(CurrentSpawnedItem))
		{
			return;
		}
	
		if(ItemToSpawnClass)
		{
			//since IngredientSpawnLocation's location is in local space, convert it to world-space
			const FVector WorldLoc = GetActorLocation() + ItemSpawnLocation;
		
			//spawn item
			AEC_Item* SpawnedItem = Cast<AEC_Item>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
				ItemToSpawnClass, FTransform(WorldLoc), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
		
			if(IsValid(SpawnedItem))
			{
				//finish spawning it
				SpawnedItem->FinishSpawning(FTransform(WorldLoc));
				CurrentSpawnedItem = SpawnedItem;

				//pause spawn timer until someone picks that item up
				GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f,
				FColor::Red, FString::Printf(TEXT("No item class set for spawner %s"), *this->GetName()));
		}
	}
}

void AEC_ItemSpawner::OnInteract(AEldenCookCharacter* InteractingChar)
{
	//checks if InteractingChar is inside InteractingCharacters array
	if(IsValid(InteractingChar) && CanInteract(InteractingChar))
	{
		//if we have a spawned item waiting to be picked and the interacting char has no item in its hands
		if(IsValid(CurrentSpawnedItem) && !InteractingChar->GetCurrentItem())
		{
			//go to the hands
			InteractingChar->SetCurrentItem(CurrentSpawnedItem);
			CurrentSpawnedItem = nullptr;
			
			GetWorldTimerManager().SetTimer(ItemSpawnCooldownTimerManager, this, &AEC_ItemSpawner::SpawnItem, ItemSpawnCooldown, true, -1.0f);
		}
	}
}
