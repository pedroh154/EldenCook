#pragma once
#include "CoreMinimal.h"
#include "Interfaces/EC_ItemHolderInterface.h"
#include "Items/EC_Item.h"
#include "EC_Plate.generated.h"

/* A item that can carry multiple items inside it */
class AEC_SerializableIngredient;
UCLASS()
class ELDENCOOK_API AEC_Plate : public AEC_Item, public IEC_ItemHolderInterface
{
	GENERATED_BODY()
public:
	AEC_Plate();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	

	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	virtual void OnInteractAnotherInteractable(IEC_InteractableInterface* Interactable) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */

	
	/* ITEM HOLDER INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual bool EquipItem(AEC_Item* ItemToEquip) override;
	virtual bool CanEquipItem() const override;
	virtual void AttachItem(AEC_Item* ItemToAttach, FName Socket = NAME_None) override;
private:
	virtual void AddItem(AEC_Item* Ingredient, bool bFromRep = false) override;
	/* ITEM HOLDER INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */

	
public:
	virtual void DrawDebugVars() override;

public:
	UFUNCTION()
	virtual void OnRep_Items();

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Items)
	TArray<AEC_Item*> Items;
	
	UPROPERTY(EditAnywhere, Category="AEC_Plate|Settings")
	FName ItemsSocketName;

	UPROPERTY(EditDefaultsOnly, Category="AEC_Plate|Settings")
	int32 Slots;

public:
	TArray<AEC_Item*> GetItems() const { return Items; }
	
};
