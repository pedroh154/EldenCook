// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/EC_InteractableInterface.h"

#include "Player/EldenCookCharacter.h"

void IEC_InteractableInterface::OnInteract()
{
}

void IEC_InteractableInterface::OnInteract(AEldenCookCharacter* InteractingChar)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue,
		FString::Printf(TEXT("Interacting with an object that has no OnInteract() implementation!")));
}

bool IEC_InteractableInterface::CanInteract(AEldenCookCharacter* InteractingChar)
{
	return true;
}
