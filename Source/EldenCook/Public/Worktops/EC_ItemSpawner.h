#pragma once

#include "CoreMinimal.h"
#include "Interfaces/EC_ItemSpawnerInterface.h"
#include "Worktops/EC_Worktop.h"
#include "EC_ItemSpawner.generated.h"

//An EC_Item spawner that spawns the EC_Item at the ItemSpawnLocation
UCLASS(Abstract)
class ELDENCOOK_API AEC_ItemSpawner : public AEC_Worktop, public IEC_ItemSpawnerInterface
{
	GENERATED_BODY()

public:
	AEC_ItemSpawner();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
	virtual void SpawnItem() override;
	
	virtual void OnInteract() override;

protected:
	UPROPERTY(EditAnywhere, Category=Settings)
	TSubclassOf<AEC_Item> ItemToSpawnClass;
	
	UPROPERTY(EditAnywhere, Category="Settings")
	float ItemSpawnCooldown;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector ItemSpawnLocation;

	UPROPERTY(Replicated)
	AEC_Item* CurrentSpawnedItem;

	FTimerHandle ItemSpawnCooldownTimerManager;
	
};
