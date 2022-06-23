// Copyright Epic Games, Inc. All Rights Reserved.

#include "EldenCook/Public/Player/EldenCookCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "EldenCook/EldenCook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EldenCook/Public/Items/EC_Item.h"
#include "Player/EC_LineTraceInteractComponent.h"
#include "Interfaces/EC_InteractableInterface.h"
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
	LineTraceInteractComponent = CreateDefaultSubobject<UEC_LineTraceInteractComponent>(TEXT("LineTraceInteractComponent"));
	LineTraceInteractComponent->SetupAttachment(GetMesh());
	LineTraceInteractComponent->TraceDistanceMultiplier = 100.0f;
	LineTraceInteractComponent->TraceCollisionChannel = COLLISION_INTERACTABLE;
	LineTraceInteractComponent->NewHitActorDelegate.AddDynamic(this, &AEldenCookCharacter::OnLineTraceHighlight);

	bReplicates = true;

	HP = 3;
}

inline void AEldenCookCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetReplicateMovement(true);
}

void AEldenCookCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
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

void AEldenCookCharacter::InputInteract()
{
	//when Interact button is pressed, check if we are hitting an interactable
	AActor* LineTraceCompActor = LineTraceInteractComponent->GetCurrentHit().GetActor();
	IEC_InteractableInterface* Interactable = Cast<IEC_InteractableInterface>(LineTraceCompActor);

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
	const IEC_InteractableInterface* LineTraceHit = Cast<IEC_InteractableInterface>(LineTraceInteractComponent->GetCurrentHit().GetActor());

	//check if the LineTrace is actually hitting the Interactable (for client sent rpc)
	if(LineTraceHit == Interactable)
	{
		Interactable->OnInteract(this);
	}
}

void AEldenCookCharacter::Server_Interact_Implementation(AActor* Interactable)
{
	OnInteract(Cast<IEC_InteractableInterface>(Interactable));
}

void AEldenCookCharacter::EquipItem(AEC_Item* Item)
{
	if(IsValid(Item))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			SetCurrentItem(Item);
		}
		else
		{
			Server_EquipItem(Item);
		}
	}
}

void AEldenCookCharacter::Server_EquipItem_Implementation(AEC_Item* Item)
{
	EquipItem(Item);
}

bool AEldenCookCharacter::Server_EquipItem_Validate(AEC_Item* Item)
{
	return true;
}

void AEldenCookCharacter::InputDropItem()
{
	if(CurrentItem)
	{
		DropItem();
	}
}

void AEldenCookCharacter::DropItem()
{
	if(IsValid(CurrentItem))
	{
		if(GetLocalRole() == ROLE_Authority)
		{
			OnDropItem();
		}
		else
		{
			Server_DropItem();
		}
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
	SetCurrentItem(nullptr);
}

void AEldenCookCharacter::SetCurrentItem(AEC_Item* NewItem)
{
	//equipping
	if(!IsValid(CurrentItem) && IsValid(NewItem))
	{
		AttachItem(NewItem);
		CurrentItem = NewItem;
		CurrentItem->SetOwner(this);
		CurrentItem->OnEquip();
	}
	//dropping
	else if(CurrentItem && !NewItem)
	{
		DetachCurrentItem();
		CurrentItem->OnUnequip();
		CurrentItem->SetOwner(nullptr);
		CurrentItem = nullptr;
	}
}

void AEldenCookCharacter::AttachItem(AEC_Item* ItemToAttach, const FName Socket)
{
	if(IsValid(ItemToAttach))
	{
		ItemToAttach->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
	}
}

void AEldenCookCharacter::DetachCurrentItem()
{
	const FDetachmentTransformRules Rules(EDetachmentRule::KeepWorld, true);
	CurrentItem->DetachFromActor(Rules);
	CurrentItem->GetMeshComponent()->SetSimulatePhysics(true);
}

void AEldenCookCharacter::OnLineTraceHighlight(AActor* Hit, AActor* Last)
{
	IEC_InteractableInterface* HitInteractable = Cast<IEC_InteractableInterface>(Hit);
	IEC_InteractableInterface* LastInteractable = Cast<IEC_InteractableInterface>(Last);
	
	if(HitInteractable)
	{
		HitInteractable->OnHighlighted(this);
	}

	if(LastInteractable)
	{
		LastInteractable->OnUnhilighted(this);
	}
}

void AEldenCookCharacter::OnRep_CurrentItem()
{
	SetCurrentItem(CurrentItem);
}



