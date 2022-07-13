#include "Recipes/EC_Recipe.h"
#include "Items/EC_SerializableIngredient.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/EC_HUD.h"
#include "Player/EldenCookPlayerController.h"

AEC_Recipe::AEC_Recipe()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AEC_Recipe::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_Recipe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEC_Recipe::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEC_Recipe, Ingredients)
}

void AEC_Recipe::Init(const TArray<FIngredient> NIngredients)
{
	Ingredients = NIngredients;

	if(GetNetMode() == NM_ListenServer)
	{
		NotifyHUD();
	}
}

void AEC_Recipe::Deliver()
{
	Destroy();
}

void AEC_Recipe::OnRep_Ingredients()
{
	NotifyHUD();
}

void AEC_Recipe::NotifyHUD()
{
	for(FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AEldenCookPlayerController* PlayerController = Cast<AEldenCookPlayerController>(*Iterator);

		if(PlayerController->IsLocalController())
		{
			PlayerController->GetEc_HUD()->BP_OnNewRecipe(this);
		}
	}
}

TArray<FIngredient> AEC_Recipe::GetIngredients() const
{
	return Ingredients;
}

FString AEC_Recipe::GetRecipeKey()
{
	FString Key = FString(TEXT(""));
	
	//add the recipe to the spawned recipes to the map, forming its key using its ingredients:
	for(int32 i = 0; i < Ingredients.Num(); ++i)
	{
		Key = Key.Append(Ingredients[i].UniqueID.ToString());
	}

	return Key;
}


