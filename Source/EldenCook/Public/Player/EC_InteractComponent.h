#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EC_InteractComponent.generated.h"

class IEC_InteractableInterface;

//broadcast every trace with the current actor hit and the last one hit. even if it's still the same as last frame, so maybe you want to check if NewHit != Last
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewActorHit, class AActor*, NewHit, AActor*, Last);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENCOOK_API UEC_InteractComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UEC_InteractComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:	
	virtual void PerformTrace();

public:
	UPROPERTY(EditAnywhere, Category="EC_InteractComponent|Settings")
	float TraceDistanceMultiplier;

	UPROPERTY(EditAnywhere, Category="EC_InteractComponent|Settings")
	float SphereRadius;
	
	UPROPERTY(EditAnywhere, Category="EC_InteractComponent|Settings")
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;

	UPROPERTY(EditAnywhere, Category="EC_InteractComponent|Settings")
	bool bDrawDebug;

	//broadcast whenever a new actor has been hit by raycast
	UPROPERTY()
	FNewActorHit NewHitActorDelegate;

protected:
	UPROPERTY(VisibleAnywhere, Category="EC_InteractComponent|Status")
	TArray<FHitResult> CurrentHit;

	//UPROPERTY(EditAnywhere, Category="EC_InteractComponent|Settings")
	//TArray<TSubclassOf<IEC_InteractableInterface>> PriorityOrder;
	
public:
	TArray<FHitResult> GetCurrentHit() { return CurrentHit; };
	IEC_InteractableInterface* GetCurrentHitInteractable();
};
