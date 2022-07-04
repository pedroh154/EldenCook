#include "EldenCook/Public/Worktops/EC_Worktop.h"
#include "Components/BoxComponent.h"
#include "EldenCook/EldenCook.h"
#include "Items/EC_Item.h"
#include "Items/EC_Plate.h"
#include "Net/UnrealNetwork.h"
#include "Player/EldenCookCharacter.h"
#include "Worktops/EC_IngredientSpawner.h"

// Sets default values
AEC_Worktop::AEC_Worktop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
}

void AEC_Worktop::BeginPlay()
{
	Super::BeginPlay();
	ApplyCustomWorktopConfig();
}

void AEC_Worktop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEC_Worktop::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEC_Worktop, CurrentItem);
}

void AEC_Worktop::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyCustomWorktopConfig();
}

/* INTERACT ----------------------------------------- START */
void AEC_Worktop::OnInteract(AEldenCookCharacter* InteractingChar)
{
	if(InteractingChar)
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			//if character is carrying an item and there is no item here
			if(InteractingChar->GetCurrentItem() && !CurrentItem)
			{
				AddItemToWorktop(InteractingChar->GetCurrentItem());
				InteractingChar->DropItem();
			}
			//if character is not carrying an item and there is an item here
			else if(!InteractingChar->GetCurrentItem() && CurrentItem)
			{
				InteractingChar->EquipItem(CurrentItem);
				RemoveCurrentItemFromWorktop();
			}
			//if character is carrying an item and there is an item here, let the items handle what should be done
			else if(InteractingChar->GetCurrentItem() && CurrentItem)
			{
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
void AEC_Worktop::AddItemToWorktop(AEC_Item* Item)
{
	if(Item) SetCurrentItem(Item, CurrentItem);
}

void AEC_Worktop::RemoveCurrentItemFromWorktop()
{
	SetCurrentItem(nullptr, CurrentItem);
}
/* ADD ITEM ----------------------------------------- END */

void AEC_Worktop::SetCurrentItem(AEC_Item* NewItem, AEC_Item* LastItem)
{
	//this function is called by clients OnRep_CurrentItem

	//adding item to worktop
	if(NewItem && !LastItem)
	{
		const FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, false);
		NewItem->AttachToComponent(RootComponent, Rules, ItemSpawnSocketName);
		
		CurrentItem = NewItem;
		CurrentItem->OnEnterWorktop(this);

		ApplyCustomWorktopConfig();
	}
	//removing item from worktop
	else if(!NewItem && LastItem)
	{
		LastItem->OnLeaveWorktop();
		LastItem->SetActorHiddenInGame(false);
		CurrentItem = nullptr;
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

void AEC_Worktop::ApplyCustomWorktopConfig()
{
	if(CurrentItem)
	{
		if(CustomConfig.CustomItemMesh)
		{
			CustomCurrentItemMeshComp = NewObject<UStaticMeshComponent>(this);
			CustomCurrentItemMeshComp->RegisterComponent();
			CustomCurrentItemMeshComp->SetStaticMesh(CustomConfig.CustomItemMesh);
			CustomCurrentItemMeshComp->SetWorldLocation(RootComponent->GetSocketLocation(ItemSpawnSocketName) + GetActorLocation());
			CurrentItem->SetActorHiddenInGame(true);
		}
		else
		{
			CurrentItem->SetActorHiddenInGame(false);
		}
	
		if(!CustomConfig.bShowItemMesh)
		{
			if(CustomCurrentItemMeshComp)
			{
				CustomCurrentItemMeshComp->SetHiddenInGame(true);
			}
			else
			{
				CurrentItem->SetActorHiddenInGame(true);
			}
		}
		else
		{
			if(CustomCurrentItemMeshComp)
			{
				CustomCurrentItemMeshComp->SetHiddenInGame(false);
			}
			else
			{
				CurrentItem->SetActorHiddenInGame(false);
			}
		}

		if(CustomConfig.CustomItemSize != FVector::ZeroVector)
		{
			if(CustomCurrentItemMeshComp) CustomCurrentItemMeshComp->SetWorldScale3D(CustomConfig.CustomItemSize);
			CurrentItem->SetActorScale3D(CustomConfig.CustomItemSize);
		}
		else
		{
			CurrentItem->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
			if(CustomCurrentItemMeshComp) CustomCurrentItemMeshComp->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
		}
	}
}

void AEC_Worktop::OnRep_CurrentItem(AEC_Item* Last)
{
	SetCurrentItem(CurrentItem, Last);
}

