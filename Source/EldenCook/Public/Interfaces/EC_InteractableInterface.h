// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "EC_InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class ELDENCOOK_API UEC_InteractableInterface : public UInterface
{
	GENERATED_BODY()
	
};

class IEC_InteractableInterface
{    
	GENERATED_BODY()

public:
	virtual void OnInteract();
};


