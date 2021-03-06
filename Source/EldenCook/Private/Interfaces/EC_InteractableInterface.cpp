#include "Interfaces/EC_InteractableInterface.h"
#include "EldenCook/EldenCook.h"
#include "Player/EldenCookCharacter.h"

FString DetermineClientSV(AActor* Actor)
{
	return Actor->GetLocalRole() == ROLE_Authority ? TEXT("SV - ") : TEXT("CL - ");
}

void IEC_InteractableInterface::OnHighlighted(AEldenCookCharacter* InteractingChar)
{
}

void IEC_InteractableInterface::OnUnhilighted(AEldenCookCharacter* InteractingChar)
{
}


void IEC_InteractableInterface::OnInteract(AEldenCookCharacter* InteractingChar)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue,
		FString::Printf(TEXT("Interacting with an object that has no OnInteract() implementation!")));
}

void IEC_InteractableInterface::OnInteractAnotherInteractable(IEC_InteractableInterface* Interactable)
{
}

bool IEC_InteractableInterface::CanInteract(AEldenCookCharacter* InteractingChar)
{
	return true;
}

void IEC_InteractableInterface::SetCurrentlyInteractable(bool bInteractable, AActor* Interactable)
{
	UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Interactable->GetRootComponent());

	if(Primitive)
	{
		!bInteractable ? Primitive->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Ignore) :
			Primitive->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Block);
	}
}

bool IEC_InteractableInterface::IsCurrentlyInteractable(AActor* Interactable)
{
	ECollisionResponse Response = ECR_Ignore;

	if(Interactable)
	{
		Response = Interactable->GetRootComponent()->GetCollisionResponseToChannel(COLLISION_INTERACTABLE);
	}
	
	return !(Response == ECollisionResponse::ECR_Ignore);
}
