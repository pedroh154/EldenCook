// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EC_Worktop.h"
#include "Engine/DataTable.h"
#include "Interfaces/EC_ItemSpawnerInterface.h"
#include "EC_IngredientSpawner.generated.h"

class AEC_SerializableIngredient;

USTRUCT(BlueprintType)
struct FIngredient : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* HUDIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCuts;
};


UCLASS()
class ELDENCOOK_API AEC_IngredientSpawner : public AEC_Worktop, public IEC_ItemSpawnerInterface
{
	GENERATED_BODY()
	
public:
	AEC_IngredientSpawner();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	

	virtual void SpawnItem() override;
	
	virtual void OnInteract() override;

private:
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_SpawnItem();

protected:
	UPROPERTY(EditAnywhere, Category="Settings")
	FDataTableRowHandle IngredientToSpawn;
	
	UPROPERTY(EditAnywhere, Category="Settings")
	float IngredientSpawnCooldown;
	
private:
	FTimerHandle IngredientSpawnCooldownDelegate;

protected:
	UPROPERTY(Replicated)
	AEC_Item* CurrentSpawnedItem;

private:
	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector IngredientSpawnLocation;
};
