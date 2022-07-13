#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EC_DeliverManager.generated.h"

class AEC_RecipeSpawner;
class AEC_Plate;
class AEC_Recipe;

UCLASS()
class ELDENCOOK_API AEC_DeliverManager : public AActor
{
	GENERATED_BODY()

public:	
	AEC_DeliverManager();

protected:
	virtual void BeginPlay() override;


public:
	virtual void Init(AEC_RecipeSpawner* MyRecipeSpawner);
	AEC_Recipe* AnalyzePlate(const AEC_Plate* Plate) const;
	

	virtual void DeliverPlate(AEC_Plate* Plate);
	
	UFUNCTION(Server, Reliable)
	virtual void Server_DeliverPlate(AEC_Plate* Plate);

protected:
	UPROPERTY(VisibleAnywhere, Category="AEC_DeliverManager|Status")
	AEC_RecipeSpawner* RecipeSpawner;
};
