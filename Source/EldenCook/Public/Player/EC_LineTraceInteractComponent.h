#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EC_LineTraceInteractComponent.generated.h"

//broadcast whenever a new actor has been hit by raycast; broadcasts even if nullptr.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewActorHit, class AActor*, NewHit, AActor*, Last);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ELDENCOOK_API UEC_LineTraceInteractComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UEC_LineTraceInteractComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void PerformTrace();

public:
	UPROPERTY(EditAnywhere, Category=Settings)
	float TraceDistanceMultiplier;
	
	UPROPERTY(EditAnywhere, Category=Settings)
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;

	UPROPERTY(EditAnywhere, Category=Settings)
	bool bDrawDebug;

	//broadcast whenever a new actor has been hit by raycast; does not broadcast if nullptr.
	UPROPERTY()
	FNewActorHit NewHitActorDelegate;

protected:
	FHitResult CurrentHit;
	
public:
	FHitResult GetCurrentHit() { return CurrentHit; };
};
