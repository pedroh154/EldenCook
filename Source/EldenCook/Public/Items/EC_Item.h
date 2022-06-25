#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "EC_Item.generated.h"

class AEldenCookCharacter;
class AEC_Worktop;

//An actor that will be attached to the player's hand or a worktop (CurrentItem* inside EC_Character)
UCLASS(Abstract)
class ELDENCOOK_API AEC_Item  : public AActor, public IEC_InteractableInterface
{
	GENERATED_BODY()
	
public:	
	AEC_Item();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:
	//needs to replicate bc if we set mesh server-side it won't be set client-side
	UPROPERTY(VisibleDefaultsOnly, Category="Components", Replicated)
	UStaticMeshComponent* MeshComponent;

protected:
	UPROPERTY(VisibleAnywhere, Category="Status")
	AEldenCookCharacter* MyPlayer;

	UPROPERTY(VisibleAnywhere, Category="Status")
	AEC_Worktop* MyWorktop;

public:	
	virtual void OnEquip(AEldenCookCharacter* Char);
	virtual void OnUnequip();
	virtual void OnEnterWorktop(AEC_Worktop* Worktop);
	virtual void OnLeaveWorktop();
	
	/* Interactable Interface */
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	/* Interactable Interface */
	
	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
};
