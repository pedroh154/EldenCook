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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// UFUNCTION()
	// virtual void OnBoxComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//
	// UFUNCTION()
	// virtual void OnBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/* interactable interface */
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnHighlighted(AEldenCookCharacter* InteractingChar) override;
	virtual void OnUnhilighted(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	/* interactable interface */
	
	virtual void SetInteractingMaterial();
	virtual void RemoveInteractingMaterial();

	virtual void AddToWorktop(AEC_Item* ItemToAdd);
	virtual void RemoveCurrentItemFromWorktop();

	UFUNCTION()
	virtual void OnRep_CurrentItem();
	
protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, Category="Status")
	TArray<AEldenCookCharacter*> InteractingCharacters;

	UPROPERTY(EditAnywhere, Category=Settings)
	UMaterialInstance* MaterialWhileInteracting;

	UPROPERTY(ReplicatedUsing=OnRep_CurrentItem)
	AEC_Item* CurrentItem;

	UPROPERTY(EditAnywhere, Meta = (MakeEditWidget = true))
	FVector ItemLocation;

private:
	UPROPERTY()
	UMaterialInterface* PreviousMaterial;
	
};
