#include "Player/EC_LineTraceInteractComponent.h"

// Sets default values for this component's properties
UEC_LineTraceInteractComponent::UEC_LineTraceInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistanceMultiplier = 1000.0f;
	TraceCollisionChannel = ECollisionChannel::ECC_WorldDynamic;
}

void UEC_LineTraceInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEC_LineTraceInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + (GetForwardVector() * 1000.0f), FColor::Red, false, 0.05f, 0, .5f);
}

FHitResult* UEC_LineTraceInteractComponent::PerformTrace() const
{
	FHitResult* HitResult = nullptr;
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bDebugQuery = true;

	const FCollisionResponseParams CollisionResponseParams;
	
	GetWorld()->LineTraceSingleByChannel(*HitResult, GetComponentLocation(),
		GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), TraceCollisionChannel, CollisionQueryParams, CollisionResponseParams);

	return HitResult;
}
