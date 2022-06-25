#include "EldenCook/Public/Items/EC_Item.h"
#include "Worktops/EC_Worktop.h"
#include "EldenCook/Public/Player/EldenCookCharacter.h"
#include "Net/UnrealNetwork.h"

AEC_Item::AEC_Item()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetIsReplicated(true);
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	
	bReplicates = true;
}

void AEC_Item::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEC_Item::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

/* INTERACT ----------------------------------------- START */
void AEC_Item::OnInteract(AEldenCookCharacter* InteractingChar)
{
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
	MyPlayer = Char;
	SetActorEnableCollision(false);
}

void AEC_Item::OnUnequip()
{
	SetActorEnableCollision(true);
	SetOwner(nullptr);
	MyPlayer = nullptr;
}

void AEC_Item::OnEnterWorktop(AEC_Worktop* Worktop)
{
	MyWorktop = Worktop;
}

void AEC_Item::OnLeaveWorktop()
{
	MyWorktop = nullptr;
}
/* EVENTS ----------------------------------------- END */




