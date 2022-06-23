#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EldenCookCharacter.generated.h"

class AEC_Item;

UCLASS(Blueprintable)
class AEldenCookCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEldenCookCharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void InputInteract();

	virtual void Interact();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_Interact();

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UEC_LineTraceInteractComponent* LineTraceInteractComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Config|Player")
	int32 HP;

	UPROPERTY(VisibleAnywhere, Category="Status")
	AEC_Item* CurrentItem;
	
public:
	virtual void AttachItem(AEC_Item* ItemToAttach, FName Socket = NAME_None);
	virtual void SetCurrentItem(AEC_Item* NewItem);
	UFUNCTION()
	virtual void OnLineTraceHighlight(AActor* Hit, AActor* Last);
	
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	AEC_Item* GetCurrentItem() const { return CurrentItem; };
};



