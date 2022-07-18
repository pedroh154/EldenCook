#include "Recipes/EC_RecipeSpawnerDeliverManager.h"
#include "Items/EC_Plate.h"
#include "Recipes/EC_Recipe.h"
#include "Recipes/EC_RecipeSpawner.h"
#include "Player/EldenCookCharacter.h"

AEC_RecipeSpawnerDeliverManager::AEC_RecipeSpawnerDeliverManager()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(false);
}

void AEC_RecipeSpawnerDeliverManager::BeginPlay()
{
	Super::BeginPlay();

	if(!MyRecipeSpawner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
			FString::Printf(TEXT("%s 's MyRecipeSpawner is null! It depends on it to work! Set it using Init()!"),*GetNameSafe(this)) );
	}
}

void AEC_RecipeSpawnerDeliverManager::Init(AEC_RecipeSpawner* RecipeSpawner)
{
	MyRecipeSpawner = RecipeSpawner;
}

AEC_Recipe* AEC_RecipeSpawnerDeliverManager::AnalyzePlate(const AEC_Plate* Plate) const
{
	if(IsValid(Plate) && IsValid(MyRecipeSpawner))
	{
		//get all items on plate.
		TArray<AEC_Item*> PlateItems = Plate->GetItems();

		//get all current spawned recipes
		const TArray<AEC_Recipe*> SpawnedRecipes = MyRecipeSpawner->GetSpawnedRecipes();
	
		if(PlateItems.Num() > 0 && SpawnedRecipes.Num() > 0)
		{
			//for each spawned recipe
			for (auto& CurrentRecipe : SpawnedRecipes)
			{
				//get CurrentRecipe ingredients
				TArray<FIngredient> CurrentRecipeIngredients = CurrentRecipe->GetIngredients();
			
				//for each item inside plate
				for(auto& Item : PlateItems)
				{
					//check if item is an ingredient
					if(const AEC_SerializableIngredient* Ingredient = Cast<AEC_SerializableIngredient>(Item))
					{
						const FIngredient* IngredientRow = Ingredient->GetIngredientRow().GetRow<FIngredient>(TEXT(""));

						if(IngredientRow)
						{
							for(int32 j = 0; j < CurrentRecipeIngredients.Num(); ++j)
							{
								if(CurrentRecipeIngredients.IsValidIndex(j))
								{
									//if it is, check if it matches any ingredients inside CurrentRecipeIngredients
									if(!CurrentRecipeIngredients[j].UniqueID.ToString().Equals(IngredientRow->UniqueID.ToString()))
									{
										//if no matches are found, it's useless to keep analyzing this recipe, go for the next.
										break;
									}
									else
									{
										CurrentRecipeIngredients.RemoveAt(j);
									}
								}
							}
						}
						else
						{
							UE_LOG(LogTemp, Fatal, TEXT("AEC_RecipeSpawnerDeliverManager::AnalyzePlate: IngredientRow was null!"))
						}
						
						if(CurrentRecipeIngredients.Num() == 0)
						{
							return CurrentRecipe;
						}
					}
				}
			}
		}
	}
	
	return nullptr;
}

void AEC_RecipeSpawnerDeliverManager::OnDeliverPlate(AEC_Plate* Plate)
{
	//analyze plate client side to make sure we can deliver it
	//so not to waste an rpc
	if(GetLocalRole() < ROLE_Authority)
	{
		if(AnalyzePlate(Plate))
		{
			Server_DeliverPlate(Plate);
		}
	}
	else
	{
		DeliverPlate(Plate);
	}
}

void AEC_RecipeSpawnerDeliverManager::Server_DeliverPlate_Implementation(AEC_Plate* Plate)
{
	DeliverPlate(Plate);
}

void AEC_RecipeSpawnerDeliverManager::DeliverPlate(AEC_Plate* Plate)
{
	if(AEC_Recipe* Recipe = AnalyzePlate(Plate))
	{
		OnRecipeDeliveredDelegate.Broadcast(Recipe, Cast<AEldenCookCharacter>(Plate->GetOwner()));
		Recipe->Deliver();
	}
}



