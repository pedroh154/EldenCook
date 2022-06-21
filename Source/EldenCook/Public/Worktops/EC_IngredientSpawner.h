// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EC_Worktop.h"
#include "Engine/DataTable.h"
#include "EC_IngredientSpawner.generated.h"

class AEC_SerializableIngredient;

USTRUCT(BlueprintType)
struct FIngredient : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* HUDIcon;
};


UCLASS()
class ELDENCOOK_API AEC_IngredientSpawner : public AEC_Worktop
{
	GENERATED_BODY()
	
public:
	AEC_IngredientSpawner();

protected:
	UPROPERTY(EditAnywhere, Category="Settings")
	FDataTableRowHandle IngredientToSpawn;

	virtual void OnInteract() override;
	
};
