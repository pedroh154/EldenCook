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
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EIngredientTypes> Type;
	
};

UCLASS(NotBlueprintable, NotPlaceable)
class ELDENCOOK_API AEC_SerializableIngredient : public AEC_Item
{
	GENERATED_BODY()
public:
	AEC_SerializableIngredient();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(BlueprintReadOnly)
	UTexture2D* HUDIcon;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EIngredientTypes> Type;

	UPROPERTY()
	bool bCuts;
	
public:
	virtual void Init(UTexture2D* NewHUDIcon, UStaticMesh* Mesh, TEnumAsByte<EIngredientTypes> IngType);
};
