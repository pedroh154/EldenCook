// Copyright Epic Games, Inc. All Rights Reserved.

#include "EldenCook/Public/Player/EldenCookCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "EldenCook/EldenCook.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EldenCook/Public/Items/EC_Item.h"
#include "Interfaces/EC_InteractableInterface.h"
#include "Player/EC_LineTraceInteractComponent.h"

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

	HP = 3;
}

inline void AEldenCookCharacter::BeginPlay()
{
	Super::BeginPlay();

	if(IsLocallyControlled())
	{
		
	}
}

void AEldenCookCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}

void AEldenCookCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AEldenCookCharacter::InputInteract);
}

void AEldenCookCharacter::InputInteract()
{
	if(GetLocalRole() < ROLE_Authority)
	{
		Server_Interact();
	}
	else
	{
		Interact();
	}
}

//will run sv only by InputInteract()
void AEldenCookCharacter::Interact()
{
	AActor* LineTraceHit = LineTraceInteractComponent->GetCurrentHit().GetActor();

	if(IsValid(LineTraceHit) && LineTraceHit->Implements<UEC_InteractableInterface>())
	{
		Cast<IEC_InteractableInterface>(LineTraceHit)->OnInteract(this);
	}
}

void AEldenCookCharacter::Server_Interact_Implementation()
{
	Interact();
}

bool AEldenCookCharacter::Server_Interact_Validate()
{
	return true;
}

void AEldenCookCharacter::OnLineTraceHighlight(AActor* Hit, AActor* Last)
{
	if(IsValid(Hit))
	{
		Cast<IEC_InteractableInterface>(Hit)->OnHighlighted(this);
	}

	if(IsValid(Last))
	{
		Cast<IEC_InteractableInterface>(Last)->OnUnhilighted(this);
	}
}

void AEldenCookCharacter::AttachItem(AEC_Item* ItemToAttach, const FName Socket)
{
	if(IsValid(ItemToAttach))
	{
		ItemToAttach->AttachToComponent(this->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, Socket);
	}
}

void AEldenCookCharacter::SetCurrentItem(AEC_Item* NewItem)
{
	if(!IsValid(CurrentItem) && IsValid(NewItem))
	{
		CurrentItem = NewItem;
		CurrentItem->SetOwner(this);
		CurrentItem->OnEquip();
	}
}


