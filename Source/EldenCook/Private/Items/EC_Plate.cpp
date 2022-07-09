#include "Items/EC_Plate.h"
#include "Net/UnrealNetwork.h"
#include "Items/EC_SerializableIngredient.h"
#include "Player/EldenCookCharacter.h"

AEC_Plate::AEC_Plate()
{
	Slots = 3;
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
			//if it is, add it to this and equip this instead
			if(AEC_Item* Item = InteractingChar->GetCurrentItem())
			{
				InteractingChar->DropItem();
				AddItem(Item);
				InteractingChar->EquipItem(this);
			}
			else
			{
				Super::OnInteract(InteractingChar);
			}
		}
	}
}

void AEC_Plate::OnEquip(AEldenCookCharacter* Char)
{
	if(IsValid(Char->GetCurrentItem()))
	{
		AddItem(Char->GetCurrentItem());
	}
	
	Super::OnEquip(Char);
}

bool AEC_Plate::OnUnequip(AEC_Item* NewItem)
{
	if(NewItem)
	{
		if(AddItem(NewItem))
		{
			return false;
		}
	}
	Super::OnUnequip(NewItem);
	return true;
}

bool AEC_Plate::AddItem(AEC_Item* Item, const bool bFromRep)
{
	if(CanAddItem(Item))
	{
		if(!bFromRep) Items.Add(Item);
		AttachItem(Item);
		Item->OnEnterPlate(this);
		return true;
	}

	return false;
}

bool AEC_Plate::CanAddItem(AEC_Item* ItemToAdd)
{
	return IsValid(ItemToAdd) && Items.Num() < Slots && ItemToAdd != this && !ItemToAdd->IsA(StaticClass());
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

void AEC_Plate::AttachItem(AEC_Item* Item)
{
	if(Item)
	{
		const bool bUseSocket = this->RootComponent->DoesSocketExist(ItemsSocketName);
		Item->AttachToComponent(this->RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, bUseSocket ? ItemsSocketName : NAME_None);
	}
}

void AEC_Plate::OnRep_Items()
{
	AddItem(Items[Items.Num() - 1], true);
}
