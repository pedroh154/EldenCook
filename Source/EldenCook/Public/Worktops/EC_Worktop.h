#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "EC_Worktop.generated.h"

class AEC_Item;
class UBoxComponent;
class AEldenCookCharacter;

USTRUCT()
struct FCustomWorktopConfig
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* CustomItemMesh;

	UPROPERTY(EditDefaultsOnly)
	FVector CustomItemSize;

	UPROPERTY(EditDefaultsOnly)
	bool bShowItemMesh;

	FCustomWorktopConfig()
	{
		/* if we want the CurrentItem to have a custom mesh as soon as it is attached to this (does not carry to item) */
		CustomItemMesh = nullptr;
		
		/* if we want the CurrentItem to have a custom size as soon as it is attached to this */
		CustomItemSize = FVector::ZeroVector;

		/* should we show the mesh of the item that is currently attached to this worktop ?
		* this is useful if we want the spawner to have the appearance of a box for example (like a box of bread where the spawned item are taken "from inside" */
		bShowItemMesh = true;
	}
};

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
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
	virtual bool CanInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnHighlighted(AEldenCookCharacter* InteractingChar) override;
	virtual void OnUnhilighted(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */

public:
	/* ADD ITEM TO WORKTOP -------------------------------------------------------------------------------------------------------------------------- START */
	virtual void SetWorktopItem(AEC_Item* Item);
	/* ADD ITEM TO WORKTOP -------------------------------------------------------------------------------------------------------------------------- END */

protected:
	virtual void SetCurrentItem(AEC_Item* NewItem, AEC_Item* LastItem);

private:
	virtual void AttachItem(AEC_Item* ItemToAttach, FName Socket = NAME_None);
	virtual void DetachCurrentItem();

public:
	virtual void SetInteractingMaterial();
	virtual void RemoveInteractingMaterial();

	virtual void DrawDebugVars();
	
public:
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- START */
	UFUNCTION()
	virtual void OnRep_CurrentItem(AEC_Item* Last);
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- END */
	
protected:
	UPROPERTY(VisibleAnywhere, Category="AEC_Worktop|Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="AEC_Worktop|Components")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, Category="AEC_Worktop|Status")
	TArray<AEldenCookCharacter*> InteractingCharacters;

	UPROPERTY(VisibleAnywhere, Category="AEC_Worktop|Status", ReplicatedUsing=OnRep_CurrentItem)
	AEC_Item* CurrentItem;

protected:
	UPROPERTY(EditAnywhere, Category="AEC_Worktop|Settings|FX")
	UMaterialInstance* MaterialWhileInteracting;

	//location where the item will be put when on this worktop
	UPROPERTY(EditAnywhere, Category="AEC_Worktop|Settings", Meta = (MakeEditWidget = true))
	FName ItemSpawnSocketName;

private:
	UPROPERTY()
	UMaterialInterface* PreviousMaterial;

public:
	AEC_Item* GetCurrentItem() const {return CurrentItem;};
};
