// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenCook/Public/Items/EC_SerializableIngredient.h"

void AEC_SerializableIngredient::Init(UTexture2D* NewHUDIcon, UStaticMesh* Mesh, bool NewCuts)
{
	this->HUDIcon = NewHUDIcon;
	MeshComponent->SetStaticMesh(Mesh);
	this->bCuts = NewCuts;
}
