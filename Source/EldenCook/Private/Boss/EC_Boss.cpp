// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/EC_Boss.h"

#include "Items/EC_Plate.h"
#include "Player/EldenCookCharacter.h"
#include "Recipes/EC_RecipeSpawnerDeliverManager.h"
#include "Recipes/EC_RecipeSpawner.h"

// Sets default values
AEC_Boss::AEC_Boss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	Box->OnComponentBeginOverlap.AddDynamic(this, &AEC_Boss::OnComponentBeginOverlap);
	Box->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AEC_Boss::BeginPlay()
{
	Super::BeginPlay();

	if(ensureAlways(RecipeSpawnerClass))
	{
		MyRecipeSpawner = GetWorld()->SpawnActor<AEC_RecipeSpawner>(RecipeSpawnerClass);
	}
}

void AEC_Boss::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{

	if(AEldenCookCharacter* Char = Cast<AEldenCookCharacter>(OtherActor))
	{
		if(Char->IsLocallyControlled())
		{
			MyRecipeSpawner->GetDeliverManager()->DeliverPlate(Cast<AEC_Plate>(Char->GetCurrentItem()));
		}
	}
}

// Called every frame
void AEC_Boss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEC_Boss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

