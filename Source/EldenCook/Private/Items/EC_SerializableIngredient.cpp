// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenCook/Public/Items/EC_SerializableIngredient.h"

AEC_SerializableIngredient::AEC_SerializableIngredient()
{
	HUDIcon = nullptr;
	bCuts = true;
	MeshComponent->SetStaticMesh(nullptr);
}

void AEC_SerializableIngredient::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_SerializableIngredient::Init(UTexture2D* NewHUDIcon, UStaticMesh* Mesh, const bool NewCuts)
{
	this->HUDIcon = NewHUDIcon;
	MeshComponent->SetStaticMesh(Mesh);
	this->bCuts = NewCuts;
}
