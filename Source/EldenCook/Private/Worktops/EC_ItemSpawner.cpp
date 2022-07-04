#include "Worktops/EC_ItemSpawner.h"
#include "Items/EC_Item.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EldenCookCharacter.h"

AEC_ItemSpawner::AEC_ItemSpawner()
{
	ItemSpawnCooldown = 0;
}

void AEC_ItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	//if multiplayer, only start the timer server-side
	if(GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(ItemSpawnCooldownTimerManager, this, &AEC_ItemSpawner::SpawnItem, ItemSpawnCooldown + 0.01f, true, -1.0f);
	}
}

void AEC_ItemSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEC_ItemSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

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
		if(!HasAnyFlags(RF_Transient))//Check if this is not a preview actor. Avoid running this twice OnConstruction(). https://forums.unrealengine.com/t/actor-onconstruction-runs-twice/349114/2
		{ 
			if(IsValid(CurrentItem))
			{
				return;
			}
	
			if(ensure(ItemToSpawnClass))
			{
				//since location is in local space, convert it to world-space
				const FVector WorldLoc = GetActorLocation() + RootComponent->GetSocketLocation(ItemSpawnSocketName);
		
				//spawn item (will replicate back to clients)
				AEC_Item* SpawnedItem = Cast<AEC_Item>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
					ItemToSpawnClass, FTransform(WorldLoc), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
		
				if(IsValid(SpawnedItem))
				{
					//finish spawning it
					UGameplayStatics::FinishSpawningActor(SpawnedItem, FTransform(WorldLoc));
					AddItemToWorktop(SpawnedItem);

					SpawnedItem->SetCurrentlyInteractable(false, SpawnedItem);

					//pause spawn timer until someone picks that item up
					GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);

					//play spawn fx if listen server; will also play on client on OnRep_CurrentSpawnedItem
					PlaySpawnFX();
				}
			}
			else
			{
				GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);
			}
		}
	}
}

/* INTERACT ----------------------------------------- START */
void AEC_ItemSpawner::OnInteract(AEldenCookCharacter* InteractingChar)
{
	if(CanInteract(InteractingChar))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			//if character is carrying an item and there is no item here
			if(InteractingChar->GetCurrentItem() && !CurrentItem)
			{
				//this will behave like a normal worktop, item will be put here but this will pause the SpawnItem timer and no further spawns will happen
				//until player takes the item out of here
				GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);
				Super::OnInteract(InteractingChar);
			}
			//if character is not carrying an item and there is an item here
			else if(!InteractingChar->GetCurrentItem() && CurrentItem)
			{
				Super::OnInteract(InteractingChar);
				GetWorldTimerManager().SetTimer(ItemSpawnCooldownTimerManager, this, &AEC_ItemSpawner::SpawnItem, ItemSpawnCooldown + 0.01f, true, -1.0f);
				//make sure we add +0.01 in case of cooldown being 0.0 otherwise timer gets invalidated.
			}
		}
	}
}

bool AEC_ItemSpawner::CanInteract(AEldenCookCharacter* InteractingChar)
{
	return Super::CanInteract(InteractingChar);
}
/* INTERACT ----------------------------------------- END */

void AEC_ItemSpawner::PlaySpawnFX_Implementation()
{
	if(GetNetMode() != NM_DedicatedServer)
	{
		for (UParticleSystem* Particle : ItemSpawnFX)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, CurrentItem->GetActorLocation());
		}

		for (USoundBase* Sound : ItemSpawnSFX)
		{
			UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, CurrentItem->GetActorLocation());
		}

		BP_PlaySpawnFX();
	}
}

void AEC_ItemSpawner::OnRep_CurrentItem(AEC_Item* Last)
{
	Super::OnRep_CurrentItem(Last);
}
