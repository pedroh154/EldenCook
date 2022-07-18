#include "EldenCook/Public/Items/EC_SerializableIngredient.h"
#include "Engine/AssetManager.h"

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
		const FIngredient* Row = IngredientRow.GetRow<FIngredient>(TEXT(""));

		if(Row)
		{
			if(UAssetManager* Manager = UAssetManager::GetIfValid())
			{
				const FStreamableDelegate LoadedDelegate = FStreamableDelegate::CreateUObject(this, &AEC_SerializableIngredient::OnPrimaryAssetLoaded, Row->DataAsset);
				const TArray<FName> Bundles;
				Manager->LoadPrimaryAsset(Row->DataAsset, Bundles, LoadedDelegate);
			}
			
			Type = Row->Type;
			HUDIcon = Row->HUDIcon;
		}
	}
}

void AEC_SerializableIngredient::OnPrimaryAssetLoaded(const FPrimaryAssetId LoadedAssetId)
{
	if (const UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		const UEC_IngredientDataAsset* Data = Cast<UEC_IngredientDataAsset>(Manager->GetPrimaryAssetObject(LoadedAssetId));

		if(Data)
		{
			MeshComponent->SetStaticMesh(Data->Mesh);
		}

	};
}

FDataTableRowHandle AEC_SerializableIngredient::GetIngredientRow() const
{
	return IngredientRow;
}

void AEC_SerializableIngredient::LazyLoad()
{
	// if(RowStruct->Mesh.IsPending())
	// {
	// 	MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, *RowStruct->Mesh.ToSoftObjectPath().ToString()));
	// }
}
