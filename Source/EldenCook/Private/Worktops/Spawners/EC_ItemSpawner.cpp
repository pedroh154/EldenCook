#include "Worktops/Spawners/EC_ItemSpawner.h"
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
	SpawnItem();
}

void AEC_ItemSpawner::SpawnItem()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		if(!HasAnyFlags(RF_Transient)) //Check if this is not a preview actor. Avoid running this twice OnConstruction(). https://forums.unrealengine.com/t/actor-onconstruction-runs-twice/349114/2
		{
			BeginSpawnItem();
			FinishSpawnItem();
		}
	}
}

void AEC_ItemSpawner::BeginSpawnItem()
{
	//do it only server side, item will replicate to clients
	if(IsValid(CurrentItem))
	{
		return;
	}

	if(ensure(ItemToSpawnClass))
	{
		//spawn item (will replicate back to clients)
		EquipItem(Cast<AEC_Item>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
			ItemToSpawnClass, RootComponent->GetSocketTransform(ItemSpawnSocketName), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this)));
	}
	else
	{
		GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);
	}
}

void AEC_ItemSpawner::FinishSpawnItem()
{
	//we don't need to call this bc EquipItem assigns a pointer to the spawned item and this apparently calls this when done
	//UGameplayStatics::FinishSpawningActor(CurrentItem, RootComponent->GetSocketTransform(ItemSpawnSocketName));
	
	CurrentItem->SetCurrentlyInteractable(false, CurrentItem);

	//pause spawn timer until someone picks that item up
	GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);
	
	ApplyCustomCurrentItemSettings();
	
	//play spawn fx if listen server; will also play on client on OnRep_CurrentSpawnedItem
	if(GetNetMode() != NM_DedicatedServer) PlaySpawnFX();
}

/* INTERACT ----------------------------------------- START */
void AEC_ItemSpawner::OnInteract(AEldenCookCharacter* InteractingChar)
{
	Super::OnInteract(InteractingChar);
	
	if(CanInteract(InteractingChar))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			//item has been picked on Super
			if(!CurrentItem)
			{
				//this will behave like a normal worktop, item will be put here but this will pause the BeginSpawnItem timer and no further spawns will happen
				//until player takes the item out of here
				GetWorldTimerManager().SetTimer(ItemSpawnCooldownTimerManager, this, &AEC_ItemSpawner::SpawnItem, ItemSpawnCooldown + 0.01f, true, -1.0f);
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

void AEC_ItemSpawner::ApplyCustomCurrentItemSettings()
{
	if(IsValid(CurrentItem))
	{
		if(IsValid(CustomConfig.CustomItemMesh))
		{
			if(!IsValid(CustomCurrentItemMeshComp))
			{
				CustomCurrentItemMeshComp = NewObject<UStaticMeshComponent>(this);
				CustomCurrentItemMeshComp->RegisterComponent();
				CustomCurrentItemMeshComp->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, ItemSpawnSocketName);
			}
			
			CustomCurrentItemMeshComp->SetStaticMesh(CustomConfig.CustomItemMesh);
			CustomCurrentItemMeshComp->SetWorldLocation(RootComponent->GetSocketLocation(ItemSpawnSocketName) + GetActorLocation());
			CurrentItem->SetActorHiddenInGame(true);
		}
		else
		{
			if(IsValid(CustomCurrentItemMeshComp)) CustomCurrentItemMeshComp->DestroyComponent();
			CurrentItem->SetActorHiddenInGame(false);
		}
	
		if(!CustomConfig.bShowItemMesh)
		{
			if(IsValid(CustomCurrentItemMeshComp))
			{
				CustomCurrentItemMeshComp->SetHiddenInGame(true);
				CustomCurrentItemMeshComp->SetVisibility(false);
			}
			else
			{
				CurrentItem->SetActorHiddenInGame(true);
			}
		}
		else
		{
			if(IsValid(CustomCurrentItemMeshComp))
			{
				CustomCurrentItemMeshComp->SetHiddenInGame(false);
				CustomCurrentItemMeshComp->SetVisibility(true);
			}
			else
			{
				CurrentItem->SetActorHiddenInGame(false);
			}
		}

		if(CustomConfig.CustomItemSize != FVector::ZeroVector)
		{
			if(IsValid(CustomCurrentItemMeshComp)) CustomCurrentItemMeshComp->SetWorldScale3D(CustomConfig.CustomItemSize);
			CurrentItem->SetActorRelativeScale3D(CustomConfig.CustomItemSize);
		}
		else
		{
			CurrentItem->SetActorRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
			if(IsValid(CustomCurrentItemMeshComp)) CustomCurrentItemMeshComp->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
	}
	else
	{
		if(IsValid(CustomCurrentItemMeshComp)) CustomCurrentItemMeshComp->DestroyComponent();
	}
}

void AEC_ItemSpawner::OnRep_CurrentItem(AEC_Item* Last)
{
	Super::OnRep_CurrentItem(Last);

	//for some reason, when setting a custom scale for CurrentItem on server using ApplyCustomCurrentItemSettings
	//it will change the relative scale to a random vector, so this is a workout to reset it.
	if(CurrentItem)
		CurrentItem->SetActorRelativeLocation(FVector::ZeroVector);
}
