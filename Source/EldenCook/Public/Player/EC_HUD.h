// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Recipes/EC_Recipe.h"
#include "EC_HUD.generated.h"

class AEC_Recipe;
UCLASS()
class ELDENCOOK_API AEC_HUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnNewRecipe(AEC_Recipe* Recipe);
	
};
