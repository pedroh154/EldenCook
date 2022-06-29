#include "Player/EC_InteractComponent.h"
#include "EldenCook/EldenCook.h"
#include "Interfaces/EC_InteractableInterface.h"

UEC_InteractComponent::UEC_InteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	TraceDistanceMultiplier = 750.0f;
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
	//set trace params
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bDebugQuery = true;
	const FCollisionResponseParams CollisionResponseParams;
	FCollisionShape Shape;
	Shape.SetSphere(SphereRadius);

	AActor* HitActor = nullptr;
	
	TArray<FHitResult> Temp;
	
	//perform trace
	GetWorld()->SweepMultiByChannel(Temp, GetComponentLocation(), GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), FQuat::Identity,
		TraceCollisionChannel, Shape, CollisionQueryParams, CollisionResponseParams);

	// if(PriorityOrder.Num() > 1 && Temp.Num() > 1)
	// {
	// 	for(int32 i = 0; i < Temp.Num(); ++i)
	// 	{
	// 		if(Temp[0].GetActor()->GetClass() != PriorityOrder[0])
	// 		{
	// 			
	// 		}
	// 	}
	// }

	//get first actor hit by the multi trace
	if(Temp.GetData())
	{
		HitActor = Temp.GetData()->GetActor();
		NewHitActorDelegate.Broadcast(HitActor, CurrentHit.GetData() ? CurrentHit.GetData()->GetActor() : nullptr);
		CurrentHit = Temp;
	}
	else
	{
		CurrentHit = TArray<FHitResult>();
	}
	
	if(bDrawDebug)
	{
		GEngine->AddOnScreenDebugMessage(-1, GetWorld()->GetDeltaSeconds(), FColor::Black,
			FString::Printf(TEXT("LINE TRACE HIT: %s"), HitActor ? *HitActor->GetName() : TEXT("null")), true, FVector2D(0.75f, 0.75f));
		
		DrawDebugSphere(GetWorld(), GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), SphereRadius, 32, FColor::Red);
	}
	
	// FHitResult Temp;
	//
	// GetWorld()->LineTraceSingleByChannel(Temp, GetComponentLocation(),
	// 	GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), TraceCollisionChannel, CollisionQueryParams, CollisionResponseParams);
	//
	// //get first actor hit by the line trace
	// HitActor = Temp.GetActor();
	//
	// //if its a different actor compared to last frame, broadcast delegate (even if nullptr)
	// if(Temp.GetActor() != HitActor) NewHitActorDelegate.Broadcast(HitActor, CurrentHit.GetData()->GetActor());
	//
	// CurrentHit.Empty();
	// CurrentHit.Add(Temp);

// 		if(bDrawDebug)
// 		{
// 			GEngine->AddOnScreenDebugMessage(-1, 0.02f, FColor::Black,
// FString::Printf(TEXT("LINE TRACE DEBUG HIT: %s"), Temp.GetComponent() ? *Temp.GetComponent()->GetName() : TEXT("null")));
//
// 			DrawDebugLine(GetWorld(), GetComponentLocation(), GetComponentLocation() + (GetForwardVector() * TraceDistanceMultiplier), FColor::Red, false, 0.05f, 0, 2.f);
// 		}
	//}
}

IEC_InteractableInterface* UEC_InteractComponent::GetCurrentHitInteractable()
{
	if(GetCurrentHit().GetData())
	{
		return Cast<IEC_InteractableInterface>(GetCurrentHit().GetData()->GetActor());
	}
	return nullptr;
}
