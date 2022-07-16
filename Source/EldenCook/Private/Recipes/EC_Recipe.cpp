#include "Recipes/EC_Recipe.h"
#include "Items/EC_SerializableIngredient.h"
#include "Net/UnrealNetwork.h"
#include "Recipes/EC_RecipeSpawner.h"

AEC_Recipe::AEC_Recipe()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AEC_Recipe::BeginPlay()
{
	Super::BeginPlay();
	
	// if(!MyRecipeSpawner)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
	// 		FString::Printf(TEXT("%s 's RecipeSpawner is null! It depends on it to work, make sure to call Init() destroying!"), *GetNameSafe(this)));
	// 	
	// 	Destroy();
	// }
	
	OnRecipeSpawnedDelegate.Broadcast(this);
}

void AEC_Recipe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEC_Recipe::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEC_Recipe, Ingredients)
}

void AEC_Recipe::Init(const TArray<FIngredient> NIngredients, AEC_RecipeSpawner* RecipeSpawner)
{
	Ingredients = NIngredients;
	MyRecipeSpawner = RecipeSpawner;
}

void AEC_Recipe::Deliver()
{
	Destroy();
}

void AEC_Recipe::OnRep_Ingredients()
{
}

TArray<FIngredient> AEC_Recipe::GetIngredients() const
{
	return Ingredients;
}

FString AEC_Recipe::GetRecipeKey()
{
	FString Key = FString(TEXT(""));
	
	//add the recipe to the spawned recipes to the map, forming its key using its ingredients:
	for(int32 i = 0; i < Ingredients.Num(); ++i)
	{
		Key = Key.Append(Ingredients[i].UniqueID.ToString());
	}

	return Key;
}


