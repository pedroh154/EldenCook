// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenCook/Public/Worktops/EC_IngredientSpawner.h"
#include "EldenCook/Public/Items/EC_SerializableIngredient.h"


AEC_IngredientSpawner::AEC_IngredientSpawner()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> IngredientsTableFinder(TEXT("DataTable'/Game/Ingredients/IngredientsTable.IngredientsTable'"));
	if(IngredientsTableFinder.Succeeded())
	{
		IngredientToSpawn.DataTable = IngredientsTableFinder.Object;
	}
}

void AEC_IngredientSpawner::OnInteract()
{
	Super::OnInteract();
}

