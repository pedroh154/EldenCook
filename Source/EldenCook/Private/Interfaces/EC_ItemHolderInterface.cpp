// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/EC_ItemHolderInterface.h"

// Add default functionality here for any IEC_ItemHolderInterface functions that are not pure virtual.
bool IEC_ItemHolderInterface::EquipItem(AEC_Item* ItemToEquip)
{
	return true;
}

bool IEC_ItemHolderInterface::CanEquipItem() const
{
	return true;
}

bool IEC_ItemHolderInterface::DropItem()
{
	return true;
}

void IEC_ItemHolderInterface::SetCurrentItem(AEC_Item* NewItem, AEC_Item* LastItem)
{
}

void IEC_ItemHolderInterface::AddItem(AEC_Item* ItemToAdd, bool bFromRep)
{
}

void IEC_ItemHolderInterface::AttachItem(AEC_Item* ItemToAttach, FName Socket)
{
}

void IEC_ItemHolderInterface::DetachCurrentItem()
{
}
