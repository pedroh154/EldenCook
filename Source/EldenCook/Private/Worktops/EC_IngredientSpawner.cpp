// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenCook/Public/Worktops/EC_IngredientSpawner.h"
#include "EldenCook/Public/Items/EC_SerializableIngredient.h"
#include "Kismet/GameplayStatics.h"


AEC_IngredientSpawner::AEC_IngredientSpawner()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> IngredientsTableFinder(TEXT("DataTable'/Game/Ingredients/IngredientsTable.IngredientsTable'"));
	if(IngredientsTableFinder.Succeeded())
	{
		IngredientToSpawn.DataTable = IngredientsTableFinder.Object;
	}
}

void AEC_IngredientSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnItem();
}

void AEC_IngredientSpawner::OnInteract()
{
	Super::OnInteract();
}

void AEC_IngredientSpawner::SpawnItem()
{
	//check if data table is all set
	if(IsValid(IngredientToSpawn.DataTable) && !IngredientToSpawn.RowName.IsNone())
	{
		//spawn ingredient item
		AEC_SerializableIngredient* SpawnedIngredient = Cast<AEC_SerializableIngredient>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
			AEC_SerializableIngredient::StaticClass(), FTransform::Identity, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));

		//if ingredient has successfully spawned
		if(IsValid(SpawnedIngredient))
		{
			//serialize it and finish spawning it
			const FIngredient* RowStruct = IngredientToSpawn.DataTable->FindRow<FIngredient>(IngredientToSpawn.RowName, TEXT(""));
			SpawnedIngredient->Init(RowStruct->HUDIcon);
			SpawnedIngredient->FinishSpawning(FTransform::Identity);
		}
	}
}

