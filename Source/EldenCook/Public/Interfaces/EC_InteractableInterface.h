// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Player/EldenCookCharacter.h"
#include "EC_InteractableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UEC_InteractableInterface : public UInterface
{
	GENERATED_BODY()
	
};

class IEC_InteractableInterface
{    
	GENERATED_BODY()

public:
	//if this obj has been "highlighted" by raycast or something
	virtual void OnHighlighted(AEldenCookCharacter* InteractingChar);
	virtual void OnUnhilighted(AEldenCookCharacter* InteractingChar);
	virtual void OnInteract();
	virtual void OnInteract(AEldenCookCharacter* InteractingChar);
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar);
};


