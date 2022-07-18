#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Info.h"
#include "Items/EC_SerializableIngredient.h"
#include "EC_RecipeSpawner.generated.h"

class AEC_Recipe;
enum EIngredientTypes;
class AEC_RecipeSpawnerDeliverManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRecipeSpawned, AEC_Recipe*, SpawnedRecipe);

USTRUCT(BlueprintType)
struct FRecipeSpawnRules : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowRepeated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=1))
	uint8 MinIngredients;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint8 MaxIngredients;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EIngredientTypes> Filter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bAllowRepeatedOfSameType;

	FRecipeSpawnRules()
	{
		bAllowRepeated = true;
		MinIngredients = 1;
		MaxIngredients = 3;
		Filter = EIngredientTypes::None;
		bAllowRepeatedOfSameType = true;
	}
	
};

class AEC_SerializableIngredient;
enum EIngredientTypes;
class AEC_Recipe;

/* A class that instantiates EC_Recipes depending on the rules set on SpawnRules.
 * Only exists on the server, but spawned EC_Recipes are replicated back to clients.
 * Also spawns an EC_DeliverManager that will handle the deliver process for recipes spawned
 * by this.
 */
UCLASS(Abstract, Blueprintable)
class ELDENCOOK_API AEC_RecipeSpawner : public AInfo
{
	GENERATED_BODY()
	
public:	
	AEC_RecipeSpawner();
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	virtual void BeginPlay() override;

public:
	//spawns a new EC_Recipe actor
	void SpawnNewRecipe();

	//gets ingredients for recipe according to SpawnRules
	virtual TArray<FIngredient> GetIngredientsForRecipe();

public:
	TArray<AEC_Recipe*> GetSpawnedRecipes();
	AEC_RecipeSpawnerDeliverManager* GetDeliverManager() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="AEC_RecipeSpawner|Status")
	AEC_RecipeSpawnerDeliverManager* DeliverManager;
	
	UPROPERTY()
	UDataTable* IngredientsDataTable;
	
	TArray<AEC_Recipe*> SpawnedRecipes;
	
	UPROPERTY(EditAnywhere, Category="AEC_RecipeSpawner|Settings")
	FRecipeSpawnRules SpawnRules;

	UPROPERTY(EditAnywhere, Category="AEC_RecipeSpawner|Settings")
	float RecipeSpawnCooldown;

public:
	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnRecipeSpawned OnRecipeSpawnedDelegate;
	
};
