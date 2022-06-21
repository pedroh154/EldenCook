// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EC_Item.h"
#include "EC_SerializableIngredient.generated.h"

UCLASS(NotBlueprintable, NotPlaceable)
class ELDENCOOK_API AEC_SerializableIngredient : public AEC_Item
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	UTexture2D* HUDIcon;
	
public:
	virtual void Init(UTexture2D* NewHUDIcon);
};
