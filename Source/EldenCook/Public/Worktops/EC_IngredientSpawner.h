// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EC_Worktop.h"
#include "EC_IngredientSpawner.generated.h"

class AEC_BaseIngredient;

UCLASS()
class ELDENCOOK_API AEC_IngredientSpawner : public AEC_Worktop
{
	GENERATED_BODY()
	
public:
	AEC_IngredientSpawner();

protected:
	UPROPERTY(EditAnywhere, Category="Settings")
	TSubclassOf<AEC_BaseIngredient> IngredientClass;
	
};
