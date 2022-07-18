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

	/* MOVEMENT -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void InputForward(float Val);
	virtual void InputRight(float Val);
	/* MOVEMENT -------------------------------------------------------------------------------------------------------------------------- END */
	
	/* INTERACT -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void InputInteract();
	virtual void Interact(IEC_InteractableInterface* Interactable);
	virtual void OnInteract(IEC_InteractableInterface* Interactable);

private:
	//for some reason setting IEC_InteractableInterface type didn't work for RPC so we receive the actor and cast it
	UFUNCTION(Server, Reliable)
	virtual void Server_Interact(AActor* Interactable);
	/* INTERACT -------------------------------------------------------------------------------------------------------------------------- END */

	
	/* EQUIP ITEM -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual bool EquipItem(AEC_Item* Item);
	bool CanEquipItem() const;
	
private:
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_EquipItem(AEC_Item* Item);
	/* EQUIP ITEM -------------------------------------------------------------------------------------------------------------------------- END */

private:
	//used on Equip and Drop item
	virtual void SetCurrentItem(AEC_Item* NewItem, AEC_Item* LastItem = nullptr);

	
	/* DROP ITEM -------------------------------------------------------------------------------------------------------------------------- START */
public:
	virtual void InputDropItem();
	virtual void DropItem();
	virtual void OnDropItem();

private:
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void Server_DropItem();
	/* DROP ITEM -------------------------------------------------------------------------------------------------------------------------- END */
	
private:
	virtual void AttachItem(AEC_Item* ItemToAttach, FName Socket = NAME_None);
	virtual void DetachCurrentItem();

public:
	UFUNCTION()
	virtual void OnLineTraceHighlight(AActor* Hit, AActor* Last);
	
	virtual void DrawDebugVars();

	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- START */
	UFUNCTION()
	virtual void OnRep_CurrentItem(AEC_Item* LastItem);
	/* REP NOTIFIERS -------------------------------------------------------------------------------------------------------------------------- END */
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AEC_Character|Components", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AEC_Character|Components", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AEC_Character|Components", meta = (AllowPrivateAccess = "true"))
	class UEC_InteractComponent* LineTraceInteractComponent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AEC_Character|Settings")
	bool bDrawDebugVars;

protected:
	UPROPERTY(EditAnywhere, Category="AEC_Character|Config")
	int32 HP;

	UPROPERTY(VisibleAnywhere, Category="AEC_Character|Status", ReplicatedUsing=OnRep_CurrentItem)
	AEC_Item* CurrentItem;

	UPROPERTY(EditDefaultsOnly, Category="AEC_Character|Settings")
	FName HandsSocketName;
	
public:
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	class AEldenCookPlayerController* GetEcController() const;
	FORCEINLINE class AEC_Item* GetCurrentItem() const { return CurrentItem; };
};





