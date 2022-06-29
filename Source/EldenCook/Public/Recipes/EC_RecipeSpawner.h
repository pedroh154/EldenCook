#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Items/EC_SerializableIngredient.h"
#include "EC_RecipeSpawner.generated.h"

class AEC_Recipe;
enum EIngredientTypes;

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

UCLASS()
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
	void SpawnNewRecipe();

	//gets ingredients for recipe according to SpawnRules
	virtual TArray<FIngredient> GetIngredientsForRecipe();

protected:
	UPROPERTY()
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere, Category="AEC_RecipeSpawner|Settings")
	FRecipeSpawnRules SpawnRules;

	UPROPERTY(EditAnywhere, Category="AEC_RecipeSpawner|Settings")
	int32 NewRecipeCooldown;

	UPROPERTY(EditAnywhere, Category="AEC_RecipeSpawner|Status")
	TArray<AEC_Recipe*> SpawnedRecipes;
	
};
