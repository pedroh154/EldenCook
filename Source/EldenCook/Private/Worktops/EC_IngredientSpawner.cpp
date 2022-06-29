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
		IngredientToSpawnHandler.DataTable = IngredientsTableFinder.Object;
	}
}

void AEC_IngredientSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AEC_IngredientSpawner::BeginPlay()
{
	bUseDataTable = CheckForDataTable();
	if(!bUseDataTable)
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%s has no data-table set. Will behave like an ItemSpawner."), *GetNameSafe(this)));
	Super::BeginPlay();
}

bool AEC_IngredientSpawner::CheckForDataTable()
{
	return IsValid(IngredientToSpawnHandler.DataTable) && !IngredientToSpawnHandler.RowName.IsNone();
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
		if(IsValid(CurrentItem))
		{
			return;
		}
		
		//find the ingredient corresponding to this spawner
		const FIngredient* RowStruct = IngredientToSpawnHandler.GetRow<FIngredient>(TEXT(""));

		if(RowStruct)
		{
			//since IngredientSpawnLocation's location is in local space, convert it to world-space
			const FVector WorldLoc = GetActorLocation() + ItemLocation;
		
			//spawn ingredient item
			AEC_SerializableIngredient* SpawnedIngredient = Cast<AEC_SerializableIngredient>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
				AEC_SerializableIngredient::StaticClass(), FTransform(WorldLoc), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));

			if(IsValid(SpawnedIngredient))
			{
				//serialize it and finish spawning it
				SpawnedIngredient->Init(RowStruct->HUDIcon, RowStruct->Mesh, RowStruct->Type);
				UGameplayStatics::FinishSpawningActor(SpawnedIngredient, FTransform(WorldLoc));
				AddItemToWorktop(SpawnedIngredient);
			
				//the spawned ingredient will be attached to the spawner, we dont want the player to be able to interact with the ingredient but with the spawner to grab it.
				SpawnedIngredient->SetCurrentlyInteractable(false, SpawnedIngredient);

				//pause spawn timer until someone picks that ingredient up
				GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);

				//play spawn fx if listen server; will also play on client on OnRep_CurrentSpawnedItem
				if(GetNetMode() == NM_ListenServer)
				{
					PlaySpawnFX();
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::Printf(TEXT("%s either is pointing to invalid row at ingredients table or has no row set. Will not spawn item"), *GetNameSafe(this)));
		}
	}
}





