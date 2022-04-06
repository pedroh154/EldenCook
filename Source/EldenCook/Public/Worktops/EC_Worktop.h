// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EC_Worktop.generated.h"

class AEC_Item;

UCLASS(Abstract)
class ELDENCOOK_API AEC_Worktop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEC_Worktop();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Status")
	AEC_Item* CurrentItem;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnInteract() PURE_VIRTUAL(AEC_Worktop::OnInteract, );
};
