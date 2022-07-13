#include "Recipes/EC_DeliverManager.h"

#include "Items/EC_Plate.h"
#include "Recipes/EC_Recipe.h"
#include "Recipes/EC_RecipeSpawner.h"

AEC_DeliverManager::AEC_DeliverManager()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorEnableCollision(false);
}

void AEC_DeliverManager::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_DeliverManager::Init(AEC_RecipeSpawner* MyRecipeSpawner)
{
	RecipeSpawner = MyRecipeSpawner;
}

AEC_Recipe* AEC_DeliverManager::AnalyzePlate(const AEC_Plate* Plate) const
{
	if(IsValid(Plate))
	{
		//get all items on plate.
		TArray<AEC_Item*> PlateItems = Plate->GetItems();

		//get all current spawned recipes
		const TMultiMap<FString, AEC_Recipe*> SpawnedRecipes = RecipeSpawner->GetSpawnedRecipes();
	
		if(PlateItems.Num() > 0 && SpawnedRecipes.Num() > 0)
		{
			//for each spawned recipe
			for (auto& CurrentRecipe : SpawnedRecipes)
			{
				//get CurrentRecipe ingredients
				TArray<FIngredient> CurrentRecipeIngredients = CurrentRecipe.Value->GetIngredients();
			
				//for each item inside plate
				for(auto& Item : PlateItems)
				{
					//check if item is an ingredient
					if(const AEC_SerializableIngredient* Ingredient = Cast<AEC_SerializableIngredient>(Item))
					{
						FIngredient* IngredientRow = Ingredient->GetIngredientRow().GetRow<FIngredient>(TEXT(""));

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
						
						if(CurrentRecipeIngredients.Num() == 0)
						{
							return CurrentRecipe.Value;
						}
					}
				}
			}
		}
	}
	
	return nullptr;
}

void AEC_DeliverManager::DeliverPlate(AEC_Plate* Plate)
{
	//if(GetLocalRole() < ROLE_Authority)
	//{
		if(AnalyzePlate(Plate))
		{
			Server_DeliverPlate(Plate);
		}
	//}
}

void AEC_DeliverManager::Server_DeliverPlate_Implementation(AEC_Plate* Plate)
{
	if(AEC_Recipe* Recipe = AnalyzePlate(Plate)) Recipe->Deliver();
}


