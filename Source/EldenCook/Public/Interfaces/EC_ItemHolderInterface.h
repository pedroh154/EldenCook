#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EC_ItemHolderInterface.generated.h"

class AEC_Item;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEC_ItemHolderInterface : public UInterface
{
	GENERATED_BODY()
};

class ELDENCOOK_API IEC_ItemHolderInterface
{
	GENERATED_BODY()

public:
	//should act as a wrapper for SetCurrentItem or AddItem
	virtual bool EquipItem(AEC_Item* ItemToEquip);
	virtual bool CanEquipItem() const;
	//should act as a wrapper for SetCurrentItem
	virtual bool DropItem();
protected:
	//if we hold only one time at a time
	virtual void SetCurrentItem(AEC_Item* NewItem, AEC_Item* LastItem = nullptr);
	//if we hold multiple items at a time
	virtual void AddItem(AEC_Item* ItemToAdd, bool bFromRep = false);
public:
	virtual void AttachItem(AEC_Item* ItemToAttach, FName Socket = NAME_None);
	virtual void DetachCurrentItem();
};
