#include "EldenCook/Public/Items/EC_Item.h"
#include "EldenCook/Public/Player/EldenCookCharacter.h"

AEC_Item::AEC_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetActorEnableCollision(false);
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

void AEC_Item::OnEquip()
{
	MyPlayer = Cast<AEldenCookCharacter>(Owner);
	MyPlayer->AttachItem(this);
}

