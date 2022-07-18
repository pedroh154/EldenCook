#include "Recipes/EC_RecipeSpawner.h"
#include "Items/EC_SerializableIngredient.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Recipes/EC_RecipeSpawnerDeliverManager.h"
#include "Recipes/EC_Recipe.h"

AEC_RecipeSpawner::AEC_RecipeSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> IngredientsTableFinder(TEXT("DataTable'/Game/Ingredients/DT_IngredientsTable.DT_IngredientsTable'"));
	
	if(IngredientsTableFinder.Succeeded())
	{
		IngredientsDataTable = IngredientsTableFinder.Object;
	}

	RecipeSpawnCooldown = 2.0f;
	
	SetActorEnableCollision(false);

	bReplicates = true;
}

void AEC_RecipeSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEC_RecipeSpawner::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if(SpawnRules.MinIngredients == 0) SpawnRules.MinIngredients = 1;
	if(SpawnRules.MinIngredients > SpawnRules.MaxIngredients) SpawnRules.MinIngredients = SpawnRules.MaxIngredients;
}

void AEC_RecipeSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	if(GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		DeliverManager = GetWorld()->SpawnActorDeferred<AEC_RecipeSpawnerDeliverManager>(AEC_RecipeSpawnerDeliverManager::StaticClass(), FTransform::Identity);
		DeliverManager->Init(this);
		UGameplayStatics::FinishSpawningActor(DeliverManager, FTransform::Identity);

		if(DeliverManager)
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, this, &AEC_RecipeSpawner::SpawnNewRecipe, RecipeSpawnCooldown + 0.01f, true, 2.0f);
		}
	}
	else
	{
		Destroy();
	}
}

void AEC_RecipeSpawner::SpawnNewRecipe()
{
	//spawn item (will replicate back to clients)
	AEC_Recipe* Recipe = Cast<AEC_Recipe>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
		AEC_Recipe::StaticClass(), FTransform::Identity, ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
	
	if(Recipe)
	{
		//init recipe with random ingredients based on this spawner's settings
		Recipe->Init(GetIngredientsForRecipe(), this);

		//get ingredients of spawned recipe
		const TArray<FIngredient> RecipeIngredients = Recipe->GetIngredients();

		if(RecipeIngredients.Num() > 0)
		{
			SpawnedRecipes.Add(Recipe);
			UGameplayStatics::FinishSpawningActor(Recipe, FTransform::Identity);

			OnRecipeSpawnedDelegate.Broadcast(Recipe);

			for (auto It = SpawnedRecipes.CreateConstIterator(); It; ++It)
			{
				//crashes sometimes
				//FPlatformMisc::LocalPrint(*FString::Printf(TEXT("%s, %s\n"), *It.Key(), *It.Value()->GetName()));
			}
		}
		else
		{
			Recipe->Destroy();
		}
	}
}

TArray<FIngredient> AEC_RecipeSpawner::GetIngredientsForRecipe()
{
	UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> Getting ingredients..."))
	
	//this array will hold all possible ingredients
	TArray<FIngredient*> PossibleIngredients;

	//this array will hold the chosen ingredients, we will populate it as we go, and will be the return value
	TArray<FIngredient> ChosenIngredients;

	//first, let's get all the possible ingredients from our ingredients table
	if(IngredientsDataTable)
	{
		//according to SpawnRules, do we want only a specific type of ingredient for this recipe spawner?
		if(SpawnRules.Filter != EIngredientTypes::None)
		{
			//get the ingredients of the type specified by SpawnRules
			FDataTableCategoryHandle IngredientsTableHandler;
			IngredientsTableHandler.DataTable = IngredientsDataTable;
			IngredientsTableHandler.ColumnName = TEXT("Type");
			IngredientsTableHandler.RowContents = UEnum::GetValueAsName<EIngredientTypes>(SpawnRules.Filter);
			IngredientsTableHandler.GetRows<FIngredient>(PossibleIngredients, TEXT(""));
		}
		else
		{
			//get all of the ingredients, independent of the type
			IngredientsDataTable->GetAllRows(TEXT(""), PossibleIngredients);
		}

		UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> Num of possible ingredients: %d"), PossibleIngredients.Num());
		UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> Current possible ingredients:"));
		for(int32 i = 0; i < PossibleIngredients.Num(); i++)
		{
			UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> %s"), *PossibleIngredients[i]->UniqueID.ToString());
		}
	}

	//make sure that we have enough possible ingredients for the max number of ingredients this recipe can have.
	//e.g.: maybe the max ingredients is 5 and we only have 3 ingredients on the possible ingredients table.
	if(SpawnRules.MaxIngredients > PossibleIngredients.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> SpawnRules.MaxIngredients > than PossibleIngredients.Num(). Setting SpawnRules.MaxIngredients = PossibleIngredients.Num()"));
		SpawnRules.MaxIngredients = PossibleIngredients.Num();
		
		//if because of this adjustments the min ingredients is now > than max, updated it to be =.
		if(SpawnRules.MinIngredients > SpawnRules.MaxIngredients) SpawnRules.MinIngredients = SpawnRules.MaxIngredients;
	}

	if(SpawnRules.MinIngredients > SpawnRules.MaxIngredients) SpawnRules.MinIngredients = SpawnRules.MaxIngredients;
		
	//according to SpawnRules, pick a random number of ingredients for this recipe
	const int32 Num = FMath::RandRange(SpawnRules.MinIngredients, SpawnRules.MaxIngredients);
	UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> recipe will have %d ingredients, randomly generated inside SpawnRules range"), Num);

	//now let's iterate over all possible ingredients, until we get the same number of ingredients that this recipe will have
	for(int32 i = 0; i < Num; i++)
	{
		if(PossibleIngredients.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> We haven't picked all ingredients for recipe but no possible ingredients are left. Recipe will have %d ingredients instead"), ChosenIngredients.Num());
			break;
		}
		//pick a random index of the possible ingredients array
		const int32 ChosenIndex = FMath::RandRange(0, PossibleIngredients.Num() - 1);

		FIngredient* ChosenIngredient = PossibleIngredients[ChosenIndex];
	
		if(ChosenIngredient)
		{
			//add it to the chosen ingredients array
			ChosenIngredients.Add(*ChosenIngredient);

			UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> Chosen Ingredient '%s' added to array"), *ChosenIngredient->UniqueID.ToString());

			//if the SpawnRules don't allow the same type of ingredient to be picked twice
			if(!SpawnRules.bAllowRepeatedOfSameType && SpawnRules.Filter == None)
			{
				UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> SpawnRules do not allow to repeat ingredients with same type, removing all %s from possible ingredients array"), *UEnum::GetValueAsString(ChosenIngredient->Type));

				//iterate over the possible ingredients array removing all ingredients of same type
				//we have to use a common int iterator bc changing the array when using object iterators generate problems
				for(int32 j = 0; j < PossibleIngredients.Num(); ++j)
				{
					if(PossibleIngredients[j] && PossibleIngredients[j] != ChosenIngredient && PossibleIngredients[j]->Type == ChosenIngredient->Type)
					{
						UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> Removed: %s"), *PossibleIngredients[j]->UniqueID.ToString());
						PossibleIngredients.RemoveAt(j);
					}
				}
			}

			//if the SpawnRules don't allow the same ingredient to be picked twice
			if(!SpawnRules.bAllowRepeated)
			{
				UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> SpawnRules do not allow to repeat the same ingredient twice, removing chosen ingredient %s from possible ingredients array"), *ChosenIngredient->UniqueID.ToString());
				PossibleIngredients.Remove(ChosenIngredient);
			}

			UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> Current possible ingredients:"));
			for(int32 y = 0; y < PossibleIngredients.Num(); y++)
			{
				UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> %s"), *PossibleIngredients[y]->UniqueID.ToString());
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("Recipe Spawner %s has not hit its MaximumIngredients but no ingredient options left to spawn"), *GetNameSafe(this)));
			break;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> All chosen ingredients for this recipe:"));
	for(int32 i = 0; i < ChosenIngredients.Num(); i++)
	{
		UE_LOG(LogTemp, Display, TEXT("AEC_RecipeSpawner::GetIngredientsForRecipe -> %s"), *ChosenIngredients[i].UniqueID.ToString());
	}
	
	return ChosenIngredients;
}

AEC_RecipeSpawnerDeliverManager* AEC_RecipeSpawner::GetDeliverManager() const
{
	return DeliverManager;
}

TArray<AEC_Recipe*> AEC_RecipeSpawner::GetSpawnedRecipes()
{
	return SpawnedRecipes;
}


