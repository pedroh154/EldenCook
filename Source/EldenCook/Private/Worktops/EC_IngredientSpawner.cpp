#include "EldenCook/Public/Worktops/EC_IngredientSpawner.h"
#include "EldenCook/Public/Items/EC_SerializableIngredient.h"
#include "Items/EC_Plate.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EldenCookCharacter.h"

AEC_IngredientSpawner::AEC_IngredientSpawner()
{
	//default value for data table:
	static ConstructorHelpers::FObjectFinder<UDataTable> IngredientsTableFinder(TEXT("DataTable'/Game/Ingredients/IngredientsTable.IngredientsTable'"));
	
	if(IngredientsTableFinder.Succeeded())
	{
		IngredientToSpawnHandler.DataTable = IngredientsTableFinder.Object;
	}
}

void AEC_IngredientSpawner::OnConstruction(const FTransform& Transform)
{
	bUseDataTable = CheckForDataTable();
	
	Super::OnConstruction(Transform);
}

void AEC_IngredientSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	bUseDataTable = CheckForDataTable();
	
	if(!bUseDataTable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow, FString::Printf(TEXT("%s has no data-table set. Will behave like an ItemSpawner."), *GetNameSafe(this)));
	}
}

void AEC_IngredientSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool AEC_IngredientSpawner::CheckForDataTable()
{
	return IsValid(IngredientToSpawnHandler.DataTable) && !IngredientToSpawnHandler.RowName.IsNone();
}

void AEC_IngredientSpawner::SpawnItem()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		if(!HasAnyFlags(RF_Transient)) //Check if this is not a preview actor. Avoid running this twice OnConstruction(). https://forums.unrealengine.com/t/actor-onconstruction-runs-twice/349114/2
		{
			if(IsValid(CurrentItem))
			{
				return;
			}
		
			if(!bUseDataTable)
			{
				Super::SpawnItem();
				return;
			}
			
			//spawn ingredient item
			AEC_SerializableIngredient* SpawnedIngredient = Cast<AEC_SerializableIngredient>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
				AEC_SerializableIngredient::StaticClass(), RootComponent->GetSocketTransform(ItemSpawnSocketName), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
	
			if(IsValid(SpawnedIngredient))
			{
				//serialize it and finish spawning it
				SpawnedIngredient->Init(IngredientToSpawnHandler);
				UGameplayStatics::FinishSpawningActor(SpawnedIngredient, RootComponent->GetSocketTransform(ItemSpawnSocketName));
		
				SetWorktopItem(SpawnedIngredient);
	
				//the spawned ingredient will be attached to the spawner, we dont want the player to be able to interact with the ingredient but with the spawner to grab it.
				SpawnedIngredient->SetCurrentlyInteractable(false, SpawnedIngredient);

				//pause spawn timer until someone picks that ingredient up
				GetWorldTimerManager().PauseTimer(ItemSpawnCooldownTimerManager);
		
				PlaySpawnFX();
			}
		}
	}
}







