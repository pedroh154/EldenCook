#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "EC_DeliverManager.generated.h"

class AEC_RecipeSpawner;
class AEC_Plate;
class AEC_Recipe;
class AEldenCookCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRecipeDelivered, AEC_Recipe*, DeliveredRecipe, AEldenCookCharacter*, Instigator);

/* Exists on client and server, responsible for delivering done recipes */
UCLASS(NotBlueprintable, NotPlaceable)
class ELDENCOOK_API AEC_DeliverManager : public AActor
{
	GENERATED_BODY()

public:	
	AEC_DeliverManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Init(AEC_RecipeSpawner* MyRecipeSpawner);

	virtual void OnDeliverPlate(AEC_Plate* Plate);
	AEC_Recipe* AnalyzePlate(const AEC_Plate* Plate) const;
	virtual void DeliverPlate(AEC_Plate* Plate);
	
	UFUNCTION(Server, Reliable)
	virtual void Server_DeliverPlate(AEC_Plate* Plate);

protected:
	UPROPERTY(VisibleAnywhere, Category="AEC_DeliverManager|Status")
	AEC_RecipeSpawner* RecipeSpawner;

public:
	FOnRecipeDelivered OnRecipeDeliveredDelegate;
};
