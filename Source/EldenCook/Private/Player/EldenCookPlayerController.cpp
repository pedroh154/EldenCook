// Copyright Epic Games, Inc. All Rights Reserved.

#include "EldenCook/Public/Player/EldenCookPlayerController.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Camera/CameraActor.h"
#include "EldenCook/Public/Player/EldenCookCharacter.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Player/EC_HUD.h"

AEldenCookPlayerController::AEldenCookPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bAutoManageActiveCameraTarget = false; //we don't want this because this brings problems to clients when settings level camera, they will switch back to pawn view target
}

bool AEldenCookPlayerController::SwitchToLevelCamera()
{
	if(IsLocalController())
	{
		//check if we have Level Camera set-up, if we do, set view target
		TArray<AActor*> Cameras;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraActor::StaticClass(), Cameras);
		
		for(int32 i = 0; i < Cameras.Num(); ++i)
		{
			if(Cameras[i] && Cameras[i]->ActorHasTag(TEXT("Level")))
			{
				SetViewTarget(Cameras[i]);
				return true;
			}
		}
	}

	return false;
}

AEC_HUD* AEldenCookPlayerController::GetEc_HUD()
{
	return Cast<AEC_HUD>(GetHUD());
}

void AEldenCookPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(GetNetMode() != NM_DedicatedServer)
	{
		//try switch to level camera. clients will switch OnRep_Pawn, bc pawn replication happens a few moments after begin play, putting it here wouldnt setviewtarget
		SwitchToLevelCamera();
	}
}

void AEldenCookPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if(bInputPressed)
	{
		FollowTime += DeltaTime;

		// Look for the touch location
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		if(bIsTouch)
		{
			GetHitResultUnderFinger(ETouchIndex::Touch1, ECC_Visibility, true, Hit);
		}
		else
		{
			GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		}
		HitLocation = Hit.Location;

		// Direct the Pawn towards that location
		APawn* const MyPawn = GetPawn();
		if(MyPawn)
		{
			FVector WorldDirection = (HitLocation - MyPawn->GetActorLocation()).GetSafeNormal();
			MyPawn->AddMovementInput(WorldDirection, 1.f, false);
		}
	}
	else
	{
		FollowTime = 0.f;
	}
}

void AEldenCookPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AEldenCookPlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AEldenCookPlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AEldenCookPlayerController::OnTouchPressed);
	InputComponent->BindTouch(EInputEvent::IE_Released, this, &AEldenCookPlayerController::OnTouchReleased);

}

void AEldenCookPlayerController::OnSetDestinationPressed()
{
	// We flag that the input is being pressed
	bInputPressed = true;
	// Just in case the character was moving because of a previous short press we stop it
	StopMovement();
}

void AEldenCookPlayerController::OnSetDestinationReleased()
{
	// Player is no longer pressing the input
	bInputPressed = false;

	// If it was a short press
	if(FollowTime <= ShortPressThreshold)
	{
		// We look for the location in the world where the player has pressed the input
		FVector HitLocation = FVector::ZeroVector;
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, true, Hit);
		HitLocation = Hit.Location;

		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, HitLocation);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, HitLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
}

void AEldenCookPlayerController::OnTouchPressed(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = true;
	OnSetDestinationPressed();
}

void AEldenCookPlayerController::OnTouchReleased(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	bIsTouch = false;
	OnSetDestinationReleased();
}

void AEldenCookPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();
	SwitchToLevelCamera();
}
