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
	Super::OnInteract(InteractingChar);
}

void AEC_Plate::OnEquip(AEldenCookCharacter* Char)
{
	Super::OnEquip(Char);

	AEC_Item* Item = Char->GetCurrentItem();
	
	if(Item)
	{
		AddItem(Item);
	}
}

void AEC_Plate::OnUnequip()
{
	Super::OnUnequip();
}

void AEC_Plate::OnInteract(AEC_Item* Item)
{
	if(Item)
	{
		AddItem(Item);
	}
}

void AEC_Plate::AddItem(AEC_Item* Item, const bool bFromRep)
{
	if(Item && CanAddItem())
	{
		if(!bFromRep) Items.Add(Item);
		AttachItem(Item);
		Item->OnEnterPlate(this);
	}
}

bool AEC_Plate::CanAddItem()
{
	return !(Items.Num() == 3);
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
