// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "EC_InteractableInterface.generated.h"

class AEldenCookCharacter;

UINTERFACE(MinimalAPI, Blueprintable)
class UEC_InteractableInterface : public UInterface
{
	GENERATED_BODY()
	
};

class IEC_InteractableInterface
{    
	GENERATED_BODY()

public:
	
	virtual void OnHighlighted(AEldenCookCharacter* InteractingChar); //if this obj has been "highlighted" by raycast or something
	virtual void OnUnhilighted(AEldenCookCharacter* InteractingChar); //if the raycast is no longer detecting this obj.
	virtual void OnInteract();
	virtual void OnInteract(AEldenCookCharacter* InteractingChar);
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar);
};


