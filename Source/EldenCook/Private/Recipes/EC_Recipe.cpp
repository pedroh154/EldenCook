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

void AEC_Recipe::Init(const TArray<FIngredient> NIngredients)
{
	Ingredients = NIngredients;

	if(GetNetMode() == NM_ListenServer)
	{
		NotifyHUD();
	}
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
			Cast<AEC_HUD>(PlayerController->GetHUD())->BP_OnNewRecipe(this);
		}
	}
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

