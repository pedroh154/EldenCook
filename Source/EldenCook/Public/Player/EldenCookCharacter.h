#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "EldenCookCharacter.generated.h"


class AEC_Item;

UCLASS(Blueprintable)
class AEldenCookCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEldenCookCharacter();

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	/* Interact ---------------------- */
	
public:
	virtual void InputInteract();
	virtual void Interact(IEC_InteractableInterface* Interactable);
	virtual void OnInteract(IEC_InteractableInterface* Interactable);

private:
	//for some reason setting IEC_InteractableInterface type didn't work for RPC so we receive the actor and cast it
	UFUNCTION(Server, Reliable)
	virtual void Server_Interact(AActor* Interactable);
	
	/* Interact ---------------------- */

	
	/* Drop Item ---------------------- */
	
public:
	virtual void InputDropItem();
	virtual void DropItem();
	virtual void OnDropItem();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_DropItem();
	
	/* Drop Item ---------------------- */


	/* Equip Item ---------------------- */
	
public:
	virtual void EquipItem(AEC_Item* Item);

private:
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_EquipItem(AEC_Item* Item);
	
	virtual void SetCurrentItem(AEC_Item* NewItem);
	
	/* Equip Item ---------------------- */

public:
	virtual void AttachItem(AEC_Item* ItemToAttach, FName Socket = NAME_None);
	virtual void DetachCurrentItem();
	
	UFUNCTION()
	virtual void OnLineTraceHighlight(AActor* Hit, AActor* Last);

	/* REP NOTIFIERS */
	UFUNCTION()
	virtual void OnRep_CurrentItem();
	/* REP NOTIFIERS */
	
protected:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UEC_LineTraceInteractComponent* LineTraceInteractComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Config|Player")
	int32 HP;

	UPROPERTY(VisibleAnywhere, Category="Status", ReplicatedUsing=OnRep_CurrentItem)
	AEC_Item* CurrentItem;
	
public:
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	AEC_Item* GetCurrentItem() const { return CurrentItem; };
};





