#include "Player/EC_InteractComponent.h"
#include "EldenCook/EldenCook.h"
#include "Interfaces/EC_InteractableInterface.h"

UEC_InteractComponent::UEC_InteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TraceDistanceMultiplier = 350.0f;
	TraceCollisionChannel = COLLISION_INTERACTABLE;
	bDrawDebug = false;
	SphereRadius = 35.0f;
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
	//set trace collision query params
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bDebugQuery = true;

	//set trace response params
	const FCollisionResponseParams CollisionResponseParams;
	FCollisionShape Shape;

	//set trace shape
	Shape.SetSphere(SphereRadius);

	//perform trace
	TArray<FHitResult> Temp;
	GetWorld()->SweepMultiByChannel(Temp, GetComponentLocation(), GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), FQuat::Identity,
		TraceCollisionChannel, Shape, CollisionQueryParams, CollisionResponseParams);

	//get first hit actor of sweep
	AActor* HitActor = Temp.Num() > 0 ? Temp[0].GetActor() : nullptr;

	//broadcast delegate with new hit and last hit only if they differ
	if((CurrentHit.Num() > 0 && HitActor != CurrentHit[0].GetActor())
		||
		HitActor)
	{
			NewHitActorDelegate.Broadcast(HitActor, CurrentHit.Num() > 0 ? CurrentHit[0].GetActor() : nullptr);
	}

	
	//update current hit array
	CurrentHit = Temp; 
	
	if(bDrawDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Black,
			FString::Printf(TEXT("LINE TRACE HIT: %s"), HitActor ? *HitActor->GetName() : TEXT("null")), true, FVector2D(0.75f, 0.75f));
		
		DrawDebugSphere(GetWorld(), GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), SphereRadius, 32, FColor::Red);
	}
}

IEC_InteractableInterface* UEC_InteractComponent::GetCurrentHitInteractable()
{
	if(GetCurrentHit().GetData())
	{
		return Cast<IEC_InteractableInterface>(GetCurrentHit().GetData()->GetActor());
	}
	
	return nullptr;
}
