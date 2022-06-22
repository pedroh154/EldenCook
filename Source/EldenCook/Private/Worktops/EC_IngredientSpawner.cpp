#include "EldenCook/Public/Worktops/EC_IngredientSpawner.h"
#include "EldenCook/Public/Items/EC_SerializableIngredient.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EldenCookCharacter.h"

AEC_IngredientSpawner::AEC_IngredientSpawner()
{
	//default value for data table:
	static ConstructorHelpers::FObjectFinder<UDataTable> IngredientsTableFinder(TEXT("DataTable'/Game/Ingredients/IngredientsTable.IngredientsTable'"));
	if(IngredientsTableFinder.Succeeded())
	{
		IngredientToSpawn.DataTable = IngredientsTableFinder.Object;
	}

	IngredientSpawnCooldown = 0;
}

void AEC_IngredientSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEC_IngredientSpawner, CurrentSpawnedItem);
}

void AEC_IngredientSpawner::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority)
	{
		GetWorldTimerManager().SetTimer(IngredientSpawnCooldownDelegate, this, &AEC_IngredientSpawner::SpawnItem, IngredientSpawnCooldown, true, -1);
	}
}

void AEC_IngredientSpawner::OnInteract()
{
	Super::OnInteract();

	for(int32 i = 0; i < InteractingCharacters.Num(); ++i)
	{
		AEldenCookCharacter* CurrChar = InteractingCharacters[i];
		
		if(IsValid(CurrChar) && !CurrChar->GetCurrentItem())
		{
			CurrChar->SetCurrentItem(CurrentSpawnedItem);
			CurrentSpawnedItem = nullptr;
			GetWorldTimerManager().SetTimer(IngredientSpawnCooldownDelegate, this, &AEC_IngredientSpawner::SpawnItem, IngredientSpawnCooldown, true, -1);
			GetWorldTimerManager().UnPauseTimer(IngredientSpawnCooldownDelegate);
		}
	}
}

void AEC_IngredientSpawner::SpawnItem()
{
	if(GetLocalRole() < ROLE_Authority)
	{
		Server_SpawnItem();
	}
	else
	{
		if(IsValid(CurrentSpawnedItem))
		{
			GetWorldTimerManager().PauseTimer(IngredientSpawnCooldownDelegate);
			return;
		}
		
		//check if data table is all set
		if(IsValid(IngredientToSpawn.DataTable) && !IngredientToSpawn.RowName.IsNone())
		{
			//find the ingredient corresponding to this spawner
			const FIngredient* RowStruct = IngredientToSpawn.DataTable->FindRow<FIngredient>(IngredientToSpawn.RowName, TEXT(""));

			//since IngredientSpawnLocation's location is in local space, convert it to world-space
			const FVector WorldLoc = GetActorLocation() + IngredientSpawnLocation;
			
			//spawn ingredient item
			AEC_SerializableIngredient* SpawnedIngredient = Cast<AEC_SerializableIngredient>(UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(),
				AEC_SerializableIngredient::StaticClass(), FTransform(WorldLoc), ESpawnActorCollisionHandlingMethod::AlwaysSpawn, this));
			
			if(IsValid(SpawnedIngredient))
			{
				//serialize it and finish spawning it
				SpawnedIngredient->Init(RowStruct->HUDIcon, RowStruct->Mesh, RowStruct->bCuts);
				SpawnedIngredient->FinishSpawning(FTransform(WorldLoc));
				CurrentSpawnedItem = SpawnedIngredient;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f,
				FColor::Red, FString::Printf(TEXT("No ingredient data table set for %s"), *this->GetName()));
		}
	}
}

void AEC_IngredientSpawner::Server_SpawnItem_Implementation()
{
	SpawnItem();
}

bool AEC_IngredientSpawner::Server_SpawnItem_Validate()
{
	return true;
}

