// Fill out your copyright notice in the Description page of Project Settings.
#include "EldenCook/Public/EC_Item.h"
#include "EldenCook/EldenCookCharacter.h"

// Sets default values
AEC_Item::AEC_Item()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEC_Item::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEC_Item::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

