#include "Player/EC_InteractComponent.h"
#include "EldenCook/EldenCook.h"
#include "Interfaces/EC_InteractableInterface.h"

UEC_InteractComponent::UEC_InteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TraceDistanceMultiplier = 1000.0f;
	TraceCollisionChannel = COLLISION_INTERACTABLE;
	bDrawDebug = false;
}

void UEC_InteractComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEC_InteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	PerformTrace();
}

void UEC_InteractComponent::PerformTrace()
{
	//perform line trace and store it here
	FHitResult Temp;
	
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bDebugQuery = true;
	const FCollisionResponseParams CollisionResponseParams;
	
	GetWorld()->LineTraceSingleByChannel(Temp, GetComponentLocation(),
		GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), TraceCollisionChannel, CollisionQueryParams, CollisionResponseParams);

	//get actor hit by line trace
	AActor* HitActor = Temp.GetActor();

	//if its a different actor compared to last frame, broadcast delegate (even if nullptr)
	if(CurrentHit.GetActor() != HitActor) NewHitActorDelegate.Broadcast(HitActor, CurrentHit.GetActor());

	//update CurrentHit
	CurrentHit = Temp;

	if(bDrawDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.02f, FColor::Black,
		FString::Printf(TEXT("LINE TRACE DEBUG HIT: %s"), Temp.GetComponent() ? *Temp.GetComponent()->GetName() : TEXT("")));

		DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), FColor::Red, false, 0.05f, 0, 2.f);
	}
}

IEC_InteractableInterface* UEC_InteractComponent::GetCurrentHitInteractable()
{
	return Cast<IEC_InteractableInterface>(GetCurrentHit().GetActor());
}
