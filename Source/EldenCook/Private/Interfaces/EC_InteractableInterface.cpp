#include "Interfaces/EC_InteractableInterface.h"

#include "EldenCook/EldenCook.h"
#include "Player/EldenCookCharacter.h"

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

void IEC_InteractableInterface::OnInteract(AEC_Item* Item)
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
