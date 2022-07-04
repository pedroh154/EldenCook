#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "EC_Item.generated.h"

class AEldenCookCharacter;
class AEC_Worktop;
class AEC_Plate;

//An actor that will be attached to the player's hand or a worktop (CurrentItem* inside EC_Character)
UCLASS(Abstract)
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
	virtual void OnInteract(AEC_Item* Item) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */
	
	/* EVENTS -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void OnEquip(AEldenCookCharacter* Char);
	virtual void OnUnequip();
	virtual void OnEnterWorktop(AEC_Worktop* Worktop);
	virtual void OnLeaveWorktop();
	virtual void OnEnterPlate(AEC_Plate* Plate);
	/* EVENTS -------------------------------------------------------------------------------------------------------------------------- START */

	virtual void DrawDebugVars();
	
protected:
	//needs to replicate bc if we set mesh server-side it won't be set client-side
	UPROPERTY(VisibleDefaultsOnly, Category="AEC_Item|Components")
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(VisibleAnywhere, Category="AEC_Item|Status")
	AEldenCookCharacter* MyPlayer;

	UPROPERTY(VisibleAnywhere, Category="AEC_Item|Status")
	AEC_Worktop* MyWorktop;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AEC_Item|Settings")
	bool bDrawDebugVars;
	
	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
