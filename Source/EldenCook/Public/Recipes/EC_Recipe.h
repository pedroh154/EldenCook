// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/EC_SerializableIngredient.h"
#include "GameFramework/Actor.h"
#include "EC_Recipe.generated.h"

class AEC_SerializableIngredient;
struct FIngredient;

UCLASS()
class ELDENCOOK_API AEC_Recipe : public AActor
{
	GENERATED_BODY()
	
public:	
	AEC_Recipe();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Init(TArray<FIngredient> NIngredients);

	UFUNCTION()
	virtual void OnRep_Ingredients();

	UFUNCTION()
	void NotifyHUD();

protected:
	UPROPERTY(VisibleAnywhere, Category="AEC_Recipe|Status", ReplicatedUsing=OnRep_Ingredients, BlueprintReadOnly)
	TArray<FIngredient> Ingredients;
};


