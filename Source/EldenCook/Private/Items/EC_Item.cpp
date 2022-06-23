#include "EldenCook/Public/Items/EC_Item.h"

#include "EldenCook/EldenCook.h"
#include "EldenCook/Public/Player/EldenCookCharacter.h"
#include "Net/UnrealNetwork.h"

AEC_Item::AEC_Item()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetIsReplicated(true);
	MeshComponent->SetCollisionProfileName(TEXT("EC_Item"));
	MeshComponent->SetCollisionObjectType(COLLISION_ITEM);
	
	SetActorEnableCollision(false); //default no collision
	
	bReplicates = true;
}

void AEC_Item::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEC_Item::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEC_Item::OnEquip()
{
	MyPlayer = Cast<AEldenCookCharacter>(Owner);
	SetActorEnableCollision(false);
}

void AEC_Item::OnUnequip()
{
	SetActorEnableCollision(true);
	MeshComponent->SetCollisionProfileName(TEXT("EC_Item"));
	MeshComponent->AddForce(FVector(MyPlayer->GetActorForwardVector() * 1000.0f));
	MyPlayer = nullptr;
}

