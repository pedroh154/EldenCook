#include "EldenCook/Public/Worktops/EC_Worktop.h"
#include "Components/BoxComponent.h"
#include "EldenCook/EldenCook.h"
#include "Items/EC_Item.h"
#include "Net/UnrealNetwork.h"
#include "Player/EldenCookCharacter.h"
#include "Worktops/EC_IngredientSpawner.h"

AEC_Worktop::AEC_Worktop()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(MeshComponent);
	BoxComponent->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));

	//since box component is not visible, let it have the interactable profile but with no physical collision
	BoxComponent->SetCollisionProfileName(TEXT("Interactable"));
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);

	bReplicates = true;

	ItemSpawnSocketName = TEXT("SOCKET_ItemSpawn");
	
	bRunConstructionScriptOnDrag = 0;
}

void AEC_Worktop::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_Worktop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugVars();
}

void AEC_Worktop::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEC_Worktop, CurrentItem);
}

void AEC_Worktop::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyCustomCurrentItemSettings();
}

/* INTERACT ----------------------------------------- START */
void AEC_Worktop::OnInteract(AEldenCookCharacter* InteractingChar)
{
	if(CanInteract(InteractingChar))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			UE_LOG(LogTemp, Display, TEXT("SV - AEC_Worktop::OnInteract: %s interacting with %s"), *GetNameSafe(InteractingChar), *GetNameSafe(this));
			AEC_Item* CharItem = InteractingChar->GetCurrentItem();
			
			//if there's no item here:
			if(!CurrentItem && InteractingChar->GetCurrentItem())
			{
				UE_LOG(LogTemp, Display, TEXT("SV - AEC_Worktop::OnInteract: %s putting item on %s"), *GetNameSafe(InteractingChar), *GetNameSafe(this));
				
				InteractingChar->DropItem(); //try to tell the char to drop its item
				SetWorktopItem(CharItem); //try to add the dropped item here
			}
			
			//if there is an item here and char has no item:
			else if (!CharItem && CurrentItem)
			{
				UE_LOG(LogTemp, Display, TEXT("SV - AEC_Worktop::OnInteract: %s taking item from %s"), *GetNameSafe(InteractingChar), *GetNameSafe(this));
				
				AEC_Item* Helper = CurrentItem;
				
				SetWorktopItem(nullptr);
				InteractingChar->EquipItem(Helper); //try to add it to the character
			}
		}
	}
}

bool AEC_Worktop::CanInteract(AEldenCookCharacter* InteractingChar)
{
	return IsValid(InteractingChar);
}

void AEC_Worktop::OnHighlighted(AEldenCookCharacter* InteractingChar)
{
	InteractingCharacters.Add(InteractingChar);

	if(InteractingChar->IsLocallyControlled())
	{
		SetInteractingMaterial();
	}
}

void AEC_Worktop::OnUnhilighted(AEldenCookCharacter* InteractingChar)
{
	InteractingCharacters.Remove(InteractingChar);

	if(InteractingChar->IsLocallyControlled())
	{
		RemoveInteractingMaterial();
	}
}
/* INTERACT ----------------------------------------- END */

void AEC_Worktop::SetInteractingMaterial()
{
	const UStaticMesh* CurrMesh = MeshComponent->GetStaticMesh();
	
    if(IsValid(MaterialWhileInteracting) && IsValid(CurrMesh))
	{
		PreviousMaterial = CurrMesh->GetMaterial(0);
		MeshComponent->GetStaticMesh()->SetMaterial(0, MaterialWhileInteracting);
	}
}

/* ADD ITEM ----------------------------------------- START */
void AEC_Worktop::SetWorktopItem(AEC_Item* Item)
{
	SetCurrentItem(Item, CurrentItem);
}

/* ADD ITEM ----------------------------------------- END */

void AEC_Worktop::SetCurrentItem(AEC_Item* NewItem, AEC_Item* LastItem)
{
	//this function is called by clients OnRep_CurrentItem
	//check SetCurrentItem for character for explanation of the logic below
	AEC_Item* LocalLastItem = nullptr;

	if(LastItem != nullptr)
	{
		LocalLastItem = LastItem;
	}
	else if(NewItem != CurrentItem)
	{
		LocalLastItem = CurrentItem;
	}

	if(GetLocalRole() == ROLE_Authority)
		UE_LOG(LogTemp, Display, TEXT("AEC_Worktop::SetCurrentItem: called on server"))
	else
		UE_LOG(LogTemp, Display, TEXT("AEC_Worktop::SetCurrentItem: called on client by onrep_ event"))
	
	
	//adding item to worktop
	if(NewItem && !LocalLastItem)
	{
		//make sure item is not owned by anyone else

		UE_LOG(LogTemp, Display, TEXT("AEC_Worktop::SetCurrentItem: %s adding to %s"), *GetNameSafe(NewItem), *GetNameSafe(this));
		
		CurrentItem = NewItem;
		CurrentItem->OnEnterWorktop(this);

		ApplyCustomCurrentItemSettings();
		AttachItem(NewItem, ItemSpawnSocketName);
	}
	//removing item from worktop
	else if(!NewItem && LocalLastItem)
	{
		UE_LOG(LogTemp, Display, TEXT("AEC_Worktop::SetCurrentItem: %s removing from %s"), *GetNameSafe(LocalLastItem), *GetNameSafe(this));
		
		LocalLastItem->OnLeaveWorktop();
		LocalLastItem->SetActorHiddenInGame(false);
		CurrentItem = nullptr;
		ApplyCustomCurrentItemSettings();
		DetachCurrentItem();
	}
}

void AEC_Worktop::AttachItem(AEC_Item* ItemToAttach, FName Socket)
{
	if(IsValid(ItemToAttach))
	{
		ItemToAttach->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
	}
}

void AEC_Worktop::DetachCurrentItem()
{
	if(CurrentItem)
	{
		const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
		CurrentItem->DetachFromActor(Rules);
	}
}

void AEC_Worktop::RemoveInteractingMaterial()
{
	UStaticMesh* CurrMesh = MeshComponent->GetStaticMesh();
	
	if(IsValid(CurrMesh))
	{
		CurrMesh->SetMaterial(0, PreviousMaterial);
	}
}

void AEC_Worktop::ApplyCustomCurrentItemSettings()
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
			CurrentItem->SetActorScale3D(CustomConfig.CustomItemSize);
		}
		else
		{
			CurrentItem->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
			//CurrentItem->SetActorRelativeLocation(FVector::ZeroVector);
			if(IsValid(CustomCurrentItemMeshComp)) CustomCurrentItemMeshComp->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
	}
	else
	{
		if(IsValid(CustomCurrentItemMeshComp)) CustomCurrentItemMeshComp->DestroyComponent();
	}
}

void AEC_Worktop::DrawDebugVars()
{
	const FVector ActorLoc = GetActorLocation();
	
	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -0), FString::Printf(TEXT("Currentitem: %s"), *GetNameSafe(CurrentItem)), nullptr,
		FColor::Green, GetWorld()->GetDeltaSeconds(), true, 1);
		
}

void AEC_Worktop::OnRep_CurrentItem(AEC_Item* Last)
{
	SetCurrentItem(CurrentItem, Last);
}

