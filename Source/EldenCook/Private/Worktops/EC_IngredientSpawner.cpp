#include "EldenCook/Public/Worktops/EC_IngredientSpawner.h"
#include "EldenCook/Public/Items/EC_SerializableIngredient.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AEC_IngredientSpawner::AEC_IngredientSpawner()
{
	//default value for data table:
	static ConstructorHelpers::FObjectFinder<UDataTable> IngredientsTableFinder(TEXT("DataTable'/Game/Ingredients/IngredientsTable.IngredientsTable'"));
	if(IngredientsTableFinder.Succeeded())
	{
		IngredientToSpawn.DataTable = IngredientsTableFinder.Object;
	}
}

void AEC_IngredientSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEC_IngredientSpawner::BeginPlay()
{
	bUseDataTable = CheckForDataTable();
	Super::BeginPlay();
}

bool AEC_IngredientSpawner::CheckForDataTable()
{
	return IsValid(IngredientToSpawn.DataTable) && !IngredientToSpawn.RowName.IsNone();
}

void AEC_IngredientSpawner::SpawnItem()
{
	//do we have the data table defined? if we do, use its settings
	bUseDataTable ? SpawnIngredient() : Super::SpawnItem();
}

void AEC_IngredientSpawner::SpawnIngredient()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		//find the ingredient corresponding to this spawner
		const FIngredient* RowStruct = IngredientToSpawn.DataTable->FindRow<FIngredient>(IngredientToSpawn.RowName, TEXT(""));

		//since IngredientSpawnLocation's location is in local space, convert it to world-space
		const FVector WorldLoc = GetActorLocation() + ItemSpawnLocation;
		
		//spawn ingredient item
		AEC_SerializableIngredient* SpawnedIngredient = Cast<AEC_SerializableIngredient>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
			AEC_SerializableIngredient::StaticClass(), FTransform(WorldLoc), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));

		if(IsValid(SpawnedIngredient))
		{
			//serialize it and finish spawning it
			SpawnedIngredient->Init(RowStruct->HUDIcon, RowStruct->Mesh, RowStruct->bCuts);
			SpawnedIngredient->FinishSpawning(FTransform(WorldLoc));
			CurrentSpawnedItem = SpawnedIngredient;

			//pause spawn timer until someone picks that ingredient up
			if(IsValid(CurrentSpawnedItem)) GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);
		}
	}
}





