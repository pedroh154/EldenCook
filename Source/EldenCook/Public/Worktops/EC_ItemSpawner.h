#pragma once

#include "CoreMinimal.h"
#include "Interfaces/EC_ItemSpawnerInterface.h"
#include "Worktops/EC_Worktop.h"
#include "EC_ItemSpawner.generated.h"

//An EC_Item spawner that spawns the EC_Item at the ItemSpawnLocation
//also works as a normal worktop, and can have items placed at the top of it
UCLASS(Abstract)
class ELDENCOOK_API AEC_ItemSpawner : public AEC_Worktop, public IEC_ItemSpawnerInterface
{
	GENERATED_BODY()

public:
	AEC_ItemSpawner();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
public:
	virtual void SpawnItem() override;
	
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */

public:
	UFUNCTION(NetMulticast, Unreliable, Category=Spawn)
	void PlaySpawnFX();

	UFUNCTION(BlueprintImplementableEvent)
	void BP_PlaySpawnFX();

	virtual void ApplyCustomCurrentItemSettings();
	
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void OnRep_CurrentItem(AEC_Item* Last) override;
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- END */
	
protected:
	UPROPERTY(EditAnywhere, Category="AEC_ItemSpawner|Settings")
	TSubclassOf<AEC_Item> ItemToSpawnClass;
	
	UPROPERTY(EditAnywhere, Category="AEC_ItemSpawner|Settings")
	float ItemSpawnCooldown;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle ItemSpawnCooldownTimerManager;

	UPROPERTY(EditAnywhere, Category="AEC_ItemSpawner|Settings|FX")
	TArray<UParticleSystem*> ItemSpawnFX;

	UPROPERTY(EditAnywhere, Category="AEC_ItemSpawner|Settings|FX")
	TArray<USoundBase*> ItemSpawnSFX;
	
	UPROPERTY(EditDefaultsOnly, Category="AEC_Worktop|Settings")
	FCustomWorktopConfig CustomConfig;

private:
	UPROPERTY()
	UStaticMeshComponent* CustomCurrentItemMeshComp;
	
};

