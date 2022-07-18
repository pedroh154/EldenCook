// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "EC_Boss.generated.h"

class AEC_RecipeSpawner;
class AEC_RecipeSpawnerDeliverManager;
UCLASS()
class ELDENCOOK_API AEC_Boss : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEC_Boss();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly)
	UBoxComponent* Box;

	UFUNCTION()
	virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category="AEC_Boss|Settings")
	TSubclassOf<AEC_RecipeSpawner> RecipeSpawnerClass;

	UPROPERTY(EditAnywhere, Category="AEC_Boss|Settings")
	AEC_RecipeSpawner* MyRecipeSpawner;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
