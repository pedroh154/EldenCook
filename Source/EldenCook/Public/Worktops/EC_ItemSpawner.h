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

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
public:
	virtual void SpawnItem() override;
	
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */

public:
	UFUNCTION(BlueprintNativeEvent, Category=Spawn)
	void PlaySpawnFX();
	
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void OnRep_CurrentItem() override;
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- END */
	
protected:
	UPROPERTY(EditAnywhere, Category="EC_ItemSpawner|Settings")
	TSubclassOf<AEC_Item> ItemToSpawnClass;
	
	UPROPERTY(EditAnywhere, Category="EC_ItemSpawner|Settings")
	float ItemSpawnCooldown;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle ItemSpawnCooldownTimerManager;

	UPROPERTY(EditAnywhere, Category="EC_ItemSpawner|Settings|FX")
	TArray<UParticleSystem*> ItemSpawnFX;

	UPROPERTY(EditAnywhere, Category="EC_ItemSpawner|Settings|FX")
	TArray<USoundBase*> ItemSpawnSFX;
	
};

