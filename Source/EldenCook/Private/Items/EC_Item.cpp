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
		if(!InteractingChar->GetCurrentItem())
		{
			InteractingChar->EquipItem(this);
		}
	}
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
	SetCurrentlyInteractable(false, this);
}

void AEC_Item::OnUnequip()
{
	SetActorEnableCollision(true);
	SetOwner(nullptr);
	MyPlayer = nullptr;
	SetCurrentlyInteractable(true, this);
}

void AEC_Item::OnEnterWorktop(AEC_Worktop* Worktop)
{
	MyWorktop = Worktop;
}

void AEC_Item::OnLeaveWorktop()
{
	MyWorktop = nullptr;
}

void AEC_Item::DrawDebugVars()
{
	const FVector ActorLoc = GetActorLocation();
	
	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -0), FString::Printf(TEXT("MyPlayer: %s"), *GetNameSafe(MyPlayer)), nullptr,
		FColor::Green, GetWorld()->GetDeltaSeconds(), true, 1);
	
	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -50.0f), FString::Printf(TEXT("MyWorktop: %s"), *GetNameSafe(MyWorktop)), nullptr,
		FColor::Blue, GetWorld()->GetDeltaSeconds(), true, 1);
}

/* EVENTS ----------------------------------------- END */




