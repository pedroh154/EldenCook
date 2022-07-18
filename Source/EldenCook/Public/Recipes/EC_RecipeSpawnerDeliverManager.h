#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "EC_RecipeSpawnerDeliverManager.generated.h"

class AEC_RecipeSpawner;
class AEC_Plate;
class AEC_Recipe;
class AEldenCookCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRecipeDelivered, AEC_Recipe*, DeliveredRecipe, AEldenCookCharacter*, Instigator);

/* Exists on client and server, responsible for delivering done recipes */
UCLASS(NotBlueprintable, NotPlaceable)
class ELDENCOOK_API AEC_RecipeSpawnerDeliverManager : public AInfo
{
	GENERATED_BODY()

public:	
	AEC_RecipeSpawnerDeliverManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Init(AEC_RecipeSpawner* MyRecipeSpawner);

/* DELIVER PLATE START */
	virtual void OnDeliverPlate(AEC_Plate* Plate);
	AEC_Recipe* AnalyzePlate(const AEC_Plate* Plate) const;
	virtual void DeliverPlate(AEC_Plate* Plate);

private:
	UFUNCTION(Server, Reliable)
	virtual void Server_DeliverPlate(AEC_Plate* Plate);
/* DELIVER PLATE END */
	
protected:
	UPROPERTY(VisibleAnywhere, Category="AEC_RecipeSpawnerDeliverManager|Status")
	AEC_RecipeSpawner* MyRecipeSpawner;

public:
	FOnRecipeDelivered OnRecipeDeliveredDelegate;
};
