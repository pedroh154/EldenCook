#pragma once

#include "CoreMinimal.h"
#include "Boss/EC_Boss.h"
#include "GameFramework/WorldSettings.h"
#include "Recipes/EC_RecipeSpawner.h"
#include "EC_WorldSettingsDefault.generated.h"

UCLASS()
class ELDENCOOK_API AEC_WorldSettingsDefault : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category=Classes)
	TArray<TSubclassOf<AEC_Boss>> WorldBossesClasses;
};
