#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "EC_LineTraceInteractComponent.generated.h"

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
	FHitResult PerformTrace() const;

public:
	UPROPERTY(EditAnywhere, Category=Settings)
	float TraceDistanceMultiplier;
	
	UPROPERTY(EditAnywhere, Category=Settings)
	TEnumAsByte<ECollisionChannel> TraceCollisionChannel;

	UPROPERTY(EditAnywhere, Category=Settings)
	bool bDrawDebug;

protected:
	FHitResult CurrentHit;
	
public:
	FHitResult GetCurrentHit() { return CurrentHit; };
};
