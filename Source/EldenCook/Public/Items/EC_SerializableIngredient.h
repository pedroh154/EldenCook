#pragma once
#include "CoreMinimal.h"
#include "EC_Item.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "EC_SerializableIngredient.generated.h"

class UTexture2D;
class UStaticMesh;

UENUM(BlueprintType)
enum EIngredientTypes
{
	None,
	Pasta,
	Vegetable,
	Meat
};

/* data that should be loaded async for every serializable ingredient */
UCLASS(BlueprintType)
class ELDENCOOK_API UEC_IngredientDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMesh* Mesh;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Ingredients", GetFName());
	}
};

/* row structure for Ingredients table */
USTRUCT(BlueprintType)
struct FIngredient : public FTableRowBase
{
	GENERATED_BODY()

public:
	/* what is the data asset that should be loaded async for this ingredient? set meshes, icons, etc here */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId DataAsset;
	
	//make it be = row name
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText UniqueID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EIngredientTypes> Type;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* HUDIcon;
	
};

/* A representation of an ingredient in the world. Must be spawned by an EC_IngredientSpawner, cannot be directly placed. */
UCLASS(NotBlueprintable, NotPlaceable)
class ELDENCOOK_API AEC_SerializableIngredient : public AEC_Item
{
	GENERATED_BODY()
public:
	AEC_SerializableIngredient();

	virtual void BeginPlay() override;

	virtual void LazyLoad();

	virtual void OnPrimaryAssetLoaded(FPrimaryAssetId LoadedAssetId);

protected:
	/* LOADS ASYNC */
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* HUDIcon;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EIngredientTypes> Type;

	UPROPERTY()
	bool bCuts;

	//what data table and which row of this data table has the data of this ingredient?
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="AEC_SerializableIngredient|Status")
	FDataTableRowHandle IngredientRow;
	
public:
	virtual void Init(FDataTableRowHandle DataTableRow);
	FDataTableRowHandle GetIngredientRow() const;
};
