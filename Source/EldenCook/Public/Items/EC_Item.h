#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EC_Item.generated.h"

class AEldenCookCharacter;

UCLASS(Abstract)
class ELDENCOOK_API AEC_Item : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEC_Item();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Status")
	AEldenCookCharacter* MyPlayer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
