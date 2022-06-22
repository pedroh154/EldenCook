#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "EC_Worktop.generated.h"

class AEC_Item;
class UBoxComponent;
class AEldenCookCharacter;

//An interactable worktop like object that stores whoever collides with its box component collision.
UCLASS(Abstract)
class ELDENCOOK_API AEC_Worktop : public AActor, public IEC_InteractableInterface
{
	GENERATED_BODY()
	
public:	
	AEC_Worktop();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnBoxComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UBoxComponent* BoxComponent;

	UPROPERTY()
	TArray<AEldenCookCharacter*> InteractingCharacters;

public:	
	// Called every frame
	
};
