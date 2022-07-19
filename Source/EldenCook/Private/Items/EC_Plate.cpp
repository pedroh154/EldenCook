#include "Items/EC_Plate.h"
#include "Net/UnrealNetwork.h"
#include "Items/EC_SerializableIngredient.h"
#include "Player/EldenCookCharacter.h"
#include "Worktops/EC_Worktop.h"

AEC_Plate::AEC_Plate()
{
	Slots = 3;
	ItemsSocketName = TEXT("SOCKET_Items");
}

void AEC_Plate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEC_Plate, Items)
}

void AEC_Plate::OnInteract(AEldenCookCharacter* InteractingChar)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		if(InteractingChar)
		{
			//check if character is holding any items
			//if it is, equip this and add item to this
			if(AEC_Item* Item = InteractingChar->GetCurrentItem())
			{
				if(CanEquipItem())
				{
					if(Item != this && !Item->IsA(StaticClass()))
					{
						InteractingChar->DropItem();
						EquipItem(Item);
						InteractingChar->EquipItem(this);
					}
				}
			}
			//if char has no items, just equip this
			else
			{
				Super::OnInteract(InteractingChar);
			}
		}
	}
}

void AEC_Plate::OnInteractAnotherInteractable(IEC_InteractableInterface* Interactable)
{
	if(AEC_Item* Item = Cast<AEC_Item>(Interactable))
	{
		if(IEC_ItemHolderInterface* Holder = Item->GetOwner<IEC_ItemHolderInterface>())
		{
			Holder->DropItem();
		}
		
		EquipItem(Item);
	}
}

bool AEC_Plate::EquipItem(AEC_Item* ItemToEquip)
{
	if(CanEquipItem())
	{
		AddItem(ItemToEquip);
		return true;
	}
	
	return false;
}

bool AEC_Plate::CanEquipItem() const
{
	return Items.Num() < Slots;
}

void AEC_Plate::AddItem(AEC_Item* Item, const bool bFromRep)
{
	if(!bFromRep)
		Items.Add(Item);
	
	AttachItem(Item);
	Item->OnEnterPlate(this);
}

void AEC_Plate::AttachItem(AEC_Item* ItemToAttach, FName Socket)
{
	if(ItemToAttach)
	{
		const bool bUseSocket = this->RootComponent->DoesSocketExist(ItemsSocketName);
		ItemToAttach->AttachToComponent(this->RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, bUseSocket ? Socket : NAME_None);
	}
}

void AEC_Plate::DrawDebugVars()
{
	Super::DrawDebugVars();
	
	const FVector ActorLoc = GetActorLocation();

	for(int32 i = 0; i < Items.Num(); i++)
	{
		DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -150.0f), FString::Printf(TEXT("Items: %s"), *GetNameSafe(Items[i])), nullptr,
			FColor::Green, GetWorld()->GetDeltaSeconds(), true, 1);
	}
}

void AEC_Plate::OnRep_Items()
{
	AddItem(Items[Items.Num() - 1], true);
}
