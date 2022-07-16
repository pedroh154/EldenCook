#pragma once
#include "CoreMinimal.h"
#include "EC_Item.h"
#include "Engine/DataTable.h"
#include "EC_SerializableIngredient.generated.h"

UENUM(BlueprintType)
enum EIngredientTypes
{
	None,
	Pasta,
	Vegetable,
	Meat
};

USTRUCT(BlueprintType)
struct FIngredient : public FTableRowBase
{
	GENERATED_BODY()

public:
	//make it be = row name
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText UniqueID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* HUDIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> Mesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EIngredientTypes> Type;
	
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

protected:
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
