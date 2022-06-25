#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "EC_Worktop.generated.h"

class AEC_Item;
class UBoxComponent;
class AEldenCookCharacter;

//An interactable worktop like object that stores whoever collides with its box component collision.
UCLASS(Abstract)
class ELDENCOOK_API AEC_Worktop : public AActor, public IEC_InteractableInterface
{
	GENERATED_BODY()

public:
	AEC_Worktop();
	
protected:	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnHighlighted(AEldenCookCharacter* InteractingChar) override;
	virtual void OnUnhilighted(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */

public:
	virtual void AddToWorktop(AEC_Item* ItemToAdd);
	virtual void RemoveCurrentItemFromWorktop();
	virtual void SetInteractingMaterial();
	virtual void RemoveInteractingMaterial();

public:
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- START */
	UFUNCTION()
	virtual void OnRep_CurrentItem();
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- END */
	
protected:
	UPROPERTY(VisibleAnywhere, Category="EC_Worktop|Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="EC_Worktop|Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, Category="EC_Worktop|Status")
	TArray<AEldenCookCharacter*> InteractingCharacters;

	UPROPERTY(VisibleAnywhere, Category="EC_Worktop|Status", ReplicatedUsing=OnRep_CurrentItem)
	AEC_Item* CurrentItem;

	UPROPERTY(EditAnywhere, Category="EC_Worktop|Settings|FX")
	UMaterialInstance* MaterialWhileInteracting;

	//location where the item will be put when on this worktop
	UPROPERTY(EditAnywhere, Category="EC_Worktop|Settings", Meta = (MakeEditWidget = true))
	FVector ItemLocation;

private:
	UPROPERTY()
	UMaterialInterface* PreviousMaterial;
	
};
