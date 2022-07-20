#include "EldenCook/Public/Worktops/Spawners/EC_IngredientSpawner.h"
#include "EldenCook/Public/Items/EC_SerializableIngredient.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AEC_IngredientSpawner::AEC_IngredientSpawner()
{
	//default value for data table:
	static ConstructorHelpers::FObjectFinder<UDataTable> IngredientsTableFinder(TEXT("DataTable'/Game/Ingredients/DT_IngredientsTable.DT_IngredientsTable'"));
	
	if(IngredientsTableFinder.Succeeded())
	{
		IngredientToSpawnHandler.DataTable = IngredientsTableFinder.Object;
	}
}

void AEC_IngredientSpawner::OnConstruction(const FTransform& Transform)
{
	bUseDataTable = CheckForDataTable();

	if(!bUseDataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%s has no data-table set. Will behave like an ItemSpawner."), *GetNameSafe(this)));
	}
	
	Super::OnConstruction(Transform);
}

void AEC_IngredientSpawner::BeginPlay()
{
	bUseDataTable = CheckForDataTable();

	if(!bUseDataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%s has no data-table set. Will behave like an ItemSpawner."), *GetNameSafe(this)));
	}
	
	Super::BeginPlay();
}

bool AEC_IngredientSpawner::CheckForDataTable()
{
	return IsValid(IngredientToSpawnHandler.DataTable) && !IngredientToSpawnHandler.RowName.IsNone();
}

void AEC_IngredientSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEC_IngredientSpawner::BeginSpawnItem()
{
	if(!HasAnyFlags(RF_Transient)) //Check if this is not a preview actor. Avoid running this twice OnConstruction(). https://forums.unrealengine.com/t/actor-onconstruction-runs-twice/349114/2
	{
		if(!bUseDataTable)
		{
			Super::BeginSpawnItem();
			return;
		}

		ItemToSpawnClass = AEC_SerializableIngredient::StaticClass();
		Super::BeginSpawnItem();
		if(AEC_SerializableIngredient* SpawnedIngredient = Cast<AEC_SerializableIngredient>(CurrentItem)) SpawnedIngredient->Init(IngredientToSpawnHandler);
	}
}






