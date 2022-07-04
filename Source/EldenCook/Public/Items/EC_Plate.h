// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/EC_Item.h"
#include "EC_Plate.generated.h"

class AEC_SerializableIngredient;
UCLASS()
class ELDENCOOK_API AEC_Plate : public AEC_Item
{
	GENERATED_BODY()

	AEC_Plate();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Items)
	TArray<AEC_Item*> Items;

	UPROPERTY(EditAnywhere, Category="AEC_Plate|Settings")
	FName ItemsSocketName;

	UPROPERTY(EditDefaultsOnly, Category="AEC_Plate|Settings")
	int32 Slots;

	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void OnInteract(AEldenCookCharacter* InteractingChar) override;
	/* INTERACTABLE INTERFACE -------------------------------------------------------------------------------------------------------------------------- END */

	virtual void OnEquip(AEldenCookCharacter* Char) override;
	virtual void OnUnequip() override;
	virtual void OnInteract(AEC_Item* Item) override;
	
public:
	virtual void AddItem(AEC_Item* Ingredient, bool bFromRep = false);
	virtual bool CanAddItem();

	virtual void DrawDebugVars() override;

private:
	virtual void AttachItem(AEC_Item* Item);

public:
	UFUNCTION()
	virtual void OnRep_Items();
	
};
