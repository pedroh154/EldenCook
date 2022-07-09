// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenCook/Public/Items/EC_SerializableIngredient.h"

AEC_SerializableIngredient::AEC_SerializableIngredient()
{
	HUDIcon = nullptr;
	bCuts = true;
	MeshComponent->SetStaticMesh(nullptr);
	RootComponent = MeshComponent;
}

void AEC_SerializableIngredient::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_SerializableIngredient::Init(UTexture2D* NewHUDIcon, UStaticMesh* Mesh, TEnumAsByte<EIngredientTypes> IngType)
{
	this->HUDIcon = NewHUDIcon;
	MeshComponent->SetStaticMesh(Mesh);
	Type = IngType;
}
