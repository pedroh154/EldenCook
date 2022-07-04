#include "EldenCook/Public/Player/EldenCookCharacter.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "EldenCook/EldenCook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EldenCook/Public/Items/EC_Item.h"
#include "Player/EC_InteractComponent.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AEldenCookCharacter::AEldenCookCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	CameraBoom->CameraLagSpeed = 0.00001; //make speed so slow that camera does not move at all (static camera)

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//Create a line trace interact component
	LineTraceInteractComponent = CreateDefaultSubobject<UEC_InteractComponent>(TEXT("LineTraceInteractComponent"));
	LineTraceInteractComponent->SetupAttachment(GetMesh());
	LineTraceInteractComponent->TraceDistanceMultiplier = 100.0f;
	LineTraceInteractComponent->TraceCollisionChannel = COLLISION_INTERACTABLE;
	LineTraceInteractComponent->NewHitActorDelegate.AddDynamic(this, &AEldenCookCharacter::OnLineTraceHighlight);

	bReplicates = true;
	
	bDrawDebugVars = false;

	HP = 3;

	HandsSocketName = TEXT("SOCKET_Hands");
}

void AEldenCookCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
}

void AEldenCookCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if(bDrawDebugVars) DrawDebugVars();
}

void AEldenCookCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AEldenCookCharacter::InputInteract);
	PlayerInputComponent->BindAction(TEXT("DropItem"), IE_Pressed, this, &AEldenCookCharacter::InputDropItem);
}

void AEldenCookCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEldenCookCharacter, CurrentItem)
}

/* INTERACT ----------------------------------------- START */
void AEldenCookCharacter::InputInteract()
{
	//when Interact button is pressed, check if we are hitting an interactable (we don't want to waste rpc calls)
	IEC_InteractableInterface* Interactable = LineTraceInteractComponent->GetCurrentHitInteractable();

	//if we are, proceed with interact
	if(Interactable) Interact(Interactable);
}

void AEldenCookCharacter::Interact(IEC_InteractableInterface* Interactable)
{
	//if we are client, send rpc to server with the Interactable we are hitting client-side
	if(GetLocalRole() < ROLE_Authority)
	{
		Server_Interact(Cast<AActor>(Interactable));
	}
	//if we are server, call OnInteract already
	else
	{
		OnInteract(Interactable);
	}
}

void AEldenCookCharacter::OnInteract(IEC_InteractableInterface* Interactable)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		const IEC_InteractableInterface* LineTraceHit = Cast<IEC_InteractableInterface>(LineTraceInteractComponent->GetCurrentHitInteractable());
		
		//check if the LineTrace is actually hitting the Interactable
		if(LineTraceHit == Interactable)
		{
			Interactable->OnInteract(this);
		}
	}

}

void AEldenCookCharacter::Server_Interact_Implementation(AActor* Interactable)
{
	OnInteract(Cast<IEC_InteractableInterface>(Interactable));
}
/* INTERACT ----------------------------------------- END */

/* EQUIP ITEM ----------------------------------------- START */
void AEldenCookCharacter::EquipItem(AEC_Item* Item)
{
	if(IsValid(Item))
	{
		if(CanEquipItem())
		{
			if(GetLocalRole() == ROLE_Authority)
			{
				SetCurrentItem(Item, CurrentItem);
			}
			else
			{
				Server_EquipItem(Item);
			}
		}
	}
}

bool AEldenCookCharacter::CanEquipItem() const
{
	return true;
}

void AEldenCookCharacter::Server_EquipItem_Implementation(AEC_Item* Item)
{
	EquipItem(Item);
}

bool AEldenCookCharacter::Server_EquipItem_Validate(AEC_Item* Item)
{
	return true;
}
/* EQUIP ITEM ----------------------------------------- END */

void AEldenCookCharacter::SetCurrentItem(AEC_Item* NewItem, AEC_Item* LastItem)
{
	//this function is called by clients OnRep_CurrentItem
	
	//equipping
	if(!IsValid(LastItem) && IsValid(NewItem))
	{
		CurrentItem = NewItem;
		CurrentItem->OnEquip(this);
		AttachItem(NewItem, HandsSocketName);
	}
	//dropping
	else if(IsValid(LastItem) && !IsValid(NewItem))
	{
		DetachCurrentItem();
		LastItem->OnUnequip();
		CurrentItem = nullptr;
	}
}

/* DROP ITEM ----------------------------------------- START */
void AEldenCookCharacter::InputDropItem()
{
	//check if we actually have an item equipped before sending rpc
	if(CurrentItem)
	{
		DropItem();
	}
}

void AEldenCookCharacter::DropItem()
{
	//send rpc if client
	if(GetLocalRole() < ROLE_Authority)
	{
		Server_DropItem();
	}
	else
	{
		OnDropItem();
	}
}

void AEldenCookCharacter::Server_DropItem_Implementation()
{
	OnDropItem();
}

bool AEldenCookCharacter::Server_DropItem_Validate()
{
	return true;
}

void AEldenCookCharacter::OnDropItem()
{
	SetCurrentItem(nullptr, CurrentItem);
}
/* DROP ITEM ----------------------------------------- END */

void AEldenCookCharacter::AttachItem(AEC_Item* ItemToAttach, const FName Socket)
{
	if(IsValid(ItemToAttach))
	{
		ItemToAttach->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
	}
}

void AEldenCookCharacter::DetachCurrentItem()
{
	if(CurrentItem)
	{
		const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
		CurrentItem->DetachFromActor(Rules);
	}
}

void AEldenCookCharacter::OnLineTraceHighlight(AActor* Hit, AActor* Last)
{
	IEC_InteractableInterface* HitInteractable = Cast<IEC_InteractableInterface>(Hit);
	IEC_InteractableInterface* LastInteractable = Cast<IEC_InteractableInterface>(Last);
	
	if(HitInteractable && HitInteractable != LastInteractable)
	{
		HitInteractable->OnHighlighted(this);
	}

	if(LastInteractable && HitInteractable != LastInteractable)
	{
		LastInteractable->OnUnhilighted(this);
	}
}

void AEldenCookCharacter::DrawDebugVars()
{
	const FVector ActorLoc = GetActorLocation();
	
	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -0.f), FString::Printf(TEXT("HP: %d"), HP), nullptr,
		FColor::Green, GetWorld()->GetDeltaSeconds(), true, 1);
	
	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -50.0f), FString::Printf(TEXT("Current Item: %s"), CurrentItem ? *CurrentItem->GetName() : TEXT("NULL")), nullptr,
		FColor::Blue, GetWorld()->GetDeltaSeconds(), true, 1);

	DrawDebugString(GetWorld(), ActorLoc + FVector(0.0f, 0.0f, -100.0f), FString::Printf(TEXT("Trace Hit: %s"), LineTraceInteractComponent->GetCurrentHitInteractable() ?
		*Cast<AActor>(LineTraceInteractComponent->GetCurrentHitInteractable())->GetName() : TEXT("NULL")), nullptr,
		FColor::Red, GetWorld()->GetDeltaSeconds(), true, 1);
}

void AEldenCookCharacter::OnRep_CurrentItem(AEC_Item* LastItem)
{
	SetCurrentItem(CurrentItem, LastItem);
}


