
#include "EldenCook/Public/Worktops/EC_Worktop.h"
#include "Components/BoxComponent.h"
#include "EldenCook/EldenCook.h"
#include "Items/EC_Item.h"
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
}

void AEC_Worktop::BeginPlay()
{
	Super::BeginPlay();
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
				AddToWorktop(InteractingChar->GetCurrentItem());
				InteractingChar->DropItem();
			}
			//if character is not carrying an item and there is an item here
			else if(!InteractingChar->GetCurrentItem() && CurrentItem)
			{
				InteractingChar->EquipItem(CurrentItem);
				RemoveCurrentItemFromWorktop();
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

void AEC_Worktop::AddToWorktop(AEC_Item* ItemToAdd)
{
	CurrentItem = ItemToAdd;
	CurrentItem->SetActorLocation(GetActorLocation() + ItemLocation);
}

void AEC_Worktop::RemoveCurrentItemFromWorktop()
{
	CurrentItem = nullptr;
}

void AEC_Worktop::SetInteractingMaterial()
{
	const UStaticMesh* CurrMesh = MeshComponent->GetStaticMesh();
	
	if(IsValid(MaterialWhileInteracting) && IsValid(CurrMesh))
	{
		PreviousMaterial = CurrMesh->GetMaterial(0);
		MeshComponent->GetStaticMesh()->SetMaterial(0, MaterialWhileInteracting);
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

void AEC_Worktop::OnRep_CurrentItem()
{

}

