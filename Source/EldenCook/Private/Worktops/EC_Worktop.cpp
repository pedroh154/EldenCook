// Fill out your copyright notice in the Description page of Project Settings.


#include "EldenCook/Public/Worktops/EC_Worktop.h"
#include "EldenCook/Public/EC_Item.h"

// Sets default values
AEC_Worktop::AEC_Worktop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEC_Worktop::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEC_Worktop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

