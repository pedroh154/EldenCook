// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EC_ItemSpawner.h"
#include "Engine/DataTable.h"
#include "EC_IngredientSpawner.generated.h"

class AEC_SerializableIngredient;

//An EC_Item spawner specialized for EC_SerializableIngredients.
//You can set a data table row handle and it will read the ingredients properties from a data table and serialize and EC_SerializableIngredient.
//if no data table is set, it will fallback to the TSubClassOf system from EC_ItemSpawner. It then will act like a normal EC_ItemSpawner.
UCLASS(Abstract)
class ELDENCOOK_API AEC_IngredientSpawner : public AEC_ItemSpawner
{
	GENERATED_BODY()
	
public:
	AEC_IngredientSpawner();
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
public:
	virtual void SpawnItem() override;
	virtual void SpawnIngredient();

private:
	virtual bool CheckForDataTable();

protected:
	UPROPERTY(EditAnywhere, Category="AEC_IngredientSpawner|Settings")
	FDataTableRowHandle IngredientToSpawnHandler;
	
private:
	bool bUseDataTable; //helper
};
