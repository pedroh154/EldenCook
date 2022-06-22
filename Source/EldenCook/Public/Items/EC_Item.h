#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EC_Item.generated.h"

class AEldenCookCharacter;

//An actor that will be attached to the player's hand or a worktop (CurrentItem* inside EC_Character)
UCLASS(Abstract)
class ELDENCOOK_API AEC_Item  : public AActor
{
	GENERATED_BODY()
	
public:	
	AEC_Item();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	
	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Status")
	AEldenCookCharacter* MyPlayer;

public:	
	virtual void OnEquip();
};
