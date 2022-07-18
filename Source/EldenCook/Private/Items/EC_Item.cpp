#include "EldenCook/Public/Items/EC_Item.h"
#include "Worktops/EC_Worktop.h"
#include "EldenCook/Public/Player/EldenCookCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Items/EC_Plate.h"

AEC_Item::AEC_Item()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("EC_Items"));
	MeshComponent->SetIsReplicated(true);
	
	bReplicates = true;
	bDrawDebugVars = false;
}

void AEC_Item::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bDrawDebugVars) DrawDebugVars();
}

void AEC_Item::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

/* INTERACT ----------------------------------------- START */
void AEC_Item::OnInteract(AEldenCookCharacter* InteractingChar)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		if(InteractingChar)
		{
			if(IsCurrentlyInteractable(this))
			{
				//if character is holding an item, let the item it is holding decide what should happen
				if(AEC_Item* CurrentCharItem = InteractingChar->GetCurrentItem())
				{
					CurrentCharItem->OnInteractAnotherInteractable(this);
				}
				//if it's not, equip this already.
				else
				{
					InteractingChar->EquipItem(this);
				}
			}
		}
	}
}

void AEC_Item::OnInteractAnotherInteractable(IEC_InteractableInterface* Interactable)
{
	IEC_InteractableInterface::OnInteractAnotherInteractable(Interactable);
}

bool AEC_Item::CanInteract(AEldenCookCharacter* InteractingChar)
{
	return true;
}
/* INTERACT ----------------------------------------- END */

/* EVENTS ----------------------------------------- START */
void AEC_Item::OnEquip(AEldenCookCharacter* Char)
{
	SetOwner(Char);
	SetActorEnableCollision(false);
	SetCurrentlyInteractable(false, this);
	UE_LOG(LogTemp, Display, TEXT("AEC_Item::OnEquip: equipped %s on %s"), *GetNameSafe(this), *GetNameSafe(Char))
}

bool AEC_Item::OnUnequip(AEC_Item* NewItem)
{
	UE_LOG(LogTemp, Display, TEXT("AEC_Item::OnUnequip: unequipped %s from %s"), *GetNameSafe(this), *GetNameSafe(Owner))
	SetActorEnableCollision(true);
	SetOwner(nullptr);
	SetCurrentlyInteractable(true, this);
	return true;
}

void AEC_Item::OnEnterWorktop(AEC_Worktop* Worktop)
{
	if(GetLocalRole() == ROLE_Authority)
		UE_LOG(LogTemp, Display, TEXT("SV - AEC_Item::OnEnterWorktop: %s entering %s"), *GetNameSafe(this), *GetNameSafe(Worktop))
	else
		UE_LOG(LogTemp, Display, TEXT("CL - AEC_Item::OnEnterWorktop: %s entering %s"), *GetNameSafe(this), *GetNameSafe(Worktop))
	
	SetOwner(Worktop);
	SetActorEnableCollision(false);
	SetCurrentlyInteractable(false, this);
}

void AEC_Item::OnLeaveWorktop()
{
	if(GetLocalRole() == ROLE_Authority)
		UE_LOG(LogTemp, Display, TEXT("SV - AEC_Item::OnLeaveWorktop: %s leaving %s"), *GetNameSafe(this), *GetNameSafe(Owner))
	else
		UE_LOG(LogTemp, Display, TEXT("CL - AEC_Item::OnLeaveWorktop: %s leaving %s"), *GetNameSafe(this), *GetNameSafe(Owner));
	
	SetOwner(nullptr);
	SetCurrentlyInteractable(true, this);
}

void AEC_Item::OnEnterPlate(AEC_Plate* Plate)
{
	SetOwner(Plate);
	SetActorEnableCollision(false);
	SetCurrentlyInteractable(false, this);
}

void AEC_Item::OnLeavePlate()
{
	SetOwner(nullptr);
	SetActorEnableCollision(true);
	SetCurrentlyInteractable(true, this);
}

void AEC_Item::DrawDebugVars()
{
	const FVector ActorLoc = GetActorLocation();
	
	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -0), FString::Printf(TEXT("Owner: %s"), *GetNameSafe(Owner)), nullptr,
		FColor::Green, GetWorld()->GetDeltaSeconds(), true, 1);
	
	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -50.0f), FString::Printf(TEXT("MyWorktop: %s"), *GetNameSafe(Owner)), nullptr,
		FColor::Blue, GetWorld()->GetDeltaSeconds(), true, 1);
}






