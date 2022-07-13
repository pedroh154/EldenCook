#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Items/EC_SerializableIngredient.h"
#include "EC_RecipeSpawner.generated.h"

class AEC_Recipe;
enum EIngredientTypes;
class AEC_DeliverManager;

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
 * Only exists on the server, but spawned EC_Recipes are replicated back to clients */
UCLASS(Abstract, Blueprintable)
class ELDENCOOK_API AEC_RecipeSpawner : public AActor
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

protected:
	UPROPERTY(EditDefaultsOnly, Category="AEC_RecipeSpawner|Settings")
	TSubclassOf<AEC_DeliverManager> DeliverManagerClass;

	UPROPERTY(EditDefaultsOnly, Category="AEC_RecipeSpawner|Status")
	AEC_DeliverManager* DeliverManager;

protected:
	UPROPERTY()
	UDataTable* DataTable;

	//TMultiMap is a TMap that allows duplicate keys
	TMultiMap<FString, AEC_Recipe*> SpawnedRecipes;
	
	UPROPERTY(EditAnywhere, Category="AEC_RecipeSpawner|Settings")
	FRecipeSpawnRules SpawnRules;

	UPROPERTY(EditAnywhere, Category="AEC_RecipeSpawner|Settings")
	float NewRecipeCooldown;

public:
	TMultiMap<FString, AEC_Recipe*> GetSpawnedRecipes();
	AEC_DeliverManager* GetDeliverManager() const;
	
};
