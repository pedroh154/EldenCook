#include "Worktops/EC_ItemSpawner.h"
#include "Items/EC_Item.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EldenCookCharacter.h"
#include "Components/BoxComponent.h"

AEC_ItemSpawner::AEC_ItemSpawner()
{
	ItemSpawnCooldown = 0;
	bReplicates = true;
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
			//since location is in local space, convert it to world-space
			const FVector WorldLoc = GetActorLocation() + ItemSpawnLocation;
		
			//spawn item (will replicate back to clients)
			AEC_Item* SpawnedItem = Cast<AEC_Item>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
				ItemToSpawnClass, FTransform(WorldLoc), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
		
			if(IsValid(SpawnedItem))
			{
				//finish spawning it
				SpawnedItem->FinishSpawning(FTransform(WorldLoc));
				CurrentSpawnedItem = SpawnedItem;

				//pause spawn timer until someone picks that item up
				GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);

				//play spawn fx if listen server; will also play on client on OnRep_CurrentSpawnedItem
				if(GetNetMode() == NM_ListenServer)
				{
					PlaySpawnFX();
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f,
				FColor::Red, FString::Printf(TEXT("No item class set for spawner %s"), *this->GetName()));
		}
	}
}

void AEC_ItemSpawner::PlaySpawnFX_Implementation()
{
	for (UParticleSystem* Particle : ItemSpawnFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, CurrentSpawnedItem->GetActorLocation());
	}

	for (USoundBase* Sound : ItemSpawnSFX)
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Sound, CurrentSpawnedItem->GetActorLocation());
	}
}

bool AEC_ItemSpawner::CanInteract(AEldenCookCharacter* InteractingChar)
{
	return Super::CanInteract(InteractingChar) && !InteractingChar->GetCurrentItem() && IsValid(CurrentSpawnedItem);
}

void AEC_ItemSpawner::OnInteract(AEldenCookCharacter* InteractingChar)
{
	if(CanInteract(InteractingChar))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			InteractingChar->EquipItem(CurrentSpawnedItem);
			CurrentSpawnedItem = nullptr;
			GetWorldTimerManager().SetTimer(ItemSpawnCooldownTimerManager, this, &AEC_ItemSpawner::SpawnItem, ItemSpawnCooldown, true, -1.0f);
		}
	}
}

void AEC_ItemSpawner::OnRep_CurrentSpawnedItem()
{
	if(CurrentSpawnedItem)
	{
		PlaySpawnFX();
	}
}
