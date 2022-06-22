#include "Player/EC_LineTraceInteractComponent.h"

#include "EldenCook/EldenCook.h"

// Sets default values for this component's properties
UEC_LineTraceInteractComponent::UEC_LineTraceInteractComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TraceDistanceMultiplier = 1000.0f;
	TraceCollisionChannel = ECC_WorldStatic;
	bDrawDebug = false;
}

void UEC_LineTraceInteractComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEC_LineTraceInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	CurrentHit = PerformTrace();
}

FHitResult UEC_LineTraceInteractComponent::PerformTrace() const
{
	FHitResult Temp;
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bDebugQuery = true;

	const FCollisionResponseParams CollisionResponseParams;
	
	GetWorld()->LineTraceSingleByChannel(Temp, GetComponentLocation(),
		GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), TraceCollisionChannel, CollisionQueryParams, CollisionResponseParams);

	if(bDrawDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.02f, FColor::Black,
		FString::Printf(TEXT("hit: %s"), Temp.GetComponent() ? *Temp.GetComponent()->GetName() : TEXT("")));

		DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + (GetForwardVector() * 1000.0f), FColor::Red, false, 0.05f, 0, .5f);
	}
	
	return Temp;
}
