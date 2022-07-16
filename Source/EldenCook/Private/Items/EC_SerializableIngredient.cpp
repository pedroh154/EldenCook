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

void AEC_SerializableIngredient::Init(const FDataTableRowHandle DataTableRow)
{
	IngredientRow = DataTableRow;

	if(IngredientRow.DataTable)
	{
		const FIngredient* RowStruct = DataTableRow.GetRow<FIngredient>(TEXT(""));

		if(RowStruct)
		{
			HUDIcon = RowStruct->HUDIcon;
			Type = RowStruct->Type;
			LazyLoad();
		}
	}
}

void AEC_SerializableIngredient::LazyLoad()
{
	const FIngredient* RowStruct = IngredientRow.GetRow<FIngredient>(TEXT(""));

	if(RowStruct->Mesh.IsPending())
	{
		MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *RowStruct->Mesh.ToSoftObjectPath().ToString()));
	}
}

FDataTableRowHandle AEC_SerializableIngredient::GetIngredientRow() const
{
	return IngredientRow;
}
