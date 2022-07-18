#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Items/EC_SerializableIngredient.h"
#include "EC_Recipe.generated.h"

class AEC_Recipe;
class AEC_SerializableIngredient;
struct FIngredient;
class AEC_RecipeSpawner;

/* A recipe that can only be instantiated by a MyRecipeSpawner.
 * Has an TArray of FIngredients (Ingredients table row struct).
 * Is replicated to clients.
 */
UCLASS(NotBlueprintable, NotPlaceable)
class ELDENCOOK_API AEC_Recipe : public AInfo
{
	GENERATED_BODY()
	
public:	
	AEC_Recipe();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Init(TArray<FIngredient> NIngredients, AEC_RecipeSpawner* RecipeSpawner);

	virtual void Deliver();

	UFUNCTION()
	virtual void OnRep_Ingredients();

protected:
	UPROPERTY(VisibleAnywhere, Category="AEC_Recipe|Status", ReplicatedUsing=OnRep_Ingredients, BlueprintReadOnly)
	TArray<FIngredient> Ingredients;
	
	UPROPERTY()
	AEC_RecipeSpawner* MyRecipeSpawner;
	
public:
	TArray<FIngredient> GetIngredients() const;
};


