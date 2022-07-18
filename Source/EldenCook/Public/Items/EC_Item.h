#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "EC_Item.generated.h"

class AEldenCookCharacter;
class AEC_Worktop;
class AEC_Plate;

//An actor that will be attached to the player's hand or a worktop (CurrentItem* inside EC_Character)
UCLASS()
class ELDENCOOK_API AEC_Item  : public AActor, public IEC_InteractableInterface
{
	GENERATED_BODY()
	
public:	
	AEC_Item();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

public:
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteractAnotherInteractable(IEC_InteractableInterface* Interactable) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */
	
	/* EVENTS -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void OnEquip(AEldenCookCharacter* Char);
	virtual bool OnUnequip(AEC_Item* NewItem);
	virtual void OnEnterWorktop(AEC_Worktop* Worktop);
	virtual void OnLeaveWorktop();
	virtual void OnEnterPlate(AEC_Plate* Plate); //item can only enter plate
	virtual void OnLeavePlate();
	/* EVENTS -------------------------------------------------------------------------------------------------------------------------- START */

	virtual void DrawDebugVars();
	
protected:
	UPROPERTY(VisibleDefaultsOnly, Category="AEC_Item|Components")
	UStaticMeshComponent* MeshComponent;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AEC_Item|Settings")
	bool bDrawDebugVars;
	
	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
