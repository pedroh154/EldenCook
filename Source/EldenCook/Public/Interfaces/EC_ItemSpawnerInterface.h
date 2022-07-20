// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EC_ItemSpawnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEC_ItemSpawnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ELDENCOOK_API IEC_ItemSpawnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SpawnItem();

protected:
	virtual void BeginSpawnItem();
	virtual void FinishSpawnItem();
};
