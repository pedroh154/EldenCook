#include "EC_Turret.h"
#include "Kismet/GameplayStatics.h"

AEC_Turret::AEC_Turret()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEC_Turret::BeginPlay()
{
	Super::BeginPlay();
}

void AEC_Turret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DetermineCurrentTarget();
	DetermineTurretState();
	DrawDebug();
}

void AEC_Turret::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AEC_Turret::DetermineCurrentTarget()
{
	if(CurrentTarget)
	{
		if(CurrentTarget->GetDistanceTo(this) > Range)
		{
			CurrentTarget = nullptr;
		}
	}
	else
	{
		if(const TArray<APawn*> Arr = GetCharactersInRange(); Arr.Num() > 0) CurrentTarget = GetCharactersInRange()[0];
		else CurrentTarget = nullptr;
	}
}

void AEC_Turret::DetermineTurretState()
{
	if(!CurrentTarget)
	{
		if(State == ETurretState::Shooting)
		{
			State = ETurretState::Idle;
			StopShooting();
		}
	}
	else
	{
		if(State == ETurretState::Idle)
		{
			State = ETurretState::Shooting;
			StartShooting();
		}
	}
}

void AEC_Turret::StartShooting()
{
	Shoot();
}

void AEC_Turret::Shoot()
{
	if(!GetWorldTimerManager().IsTimerActive(TimeBetweenShotsHandle))
	{
		GetWorldTimerManager().SetTimer(TimeBetweenShotsHandle, this, &AEC_Turret::Shoot, ShotRate, true, FirstShotDelay);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Shoot!"));
	}
}

void AEC_Turret::StopShooting()
{
	State = ETurretState::Idle;
	GetWorldTimerManager().ClearTimer(TimeBetweenShotsHandle);
}

TArray<APawn*> AEC_Turret::GetCharactersInRange() const
{
	TArray<APawn*> PawnsInRange;
	FVector MyPos = GetActorLocation();
	
	if(DetectionMethod == EDetectionMethod::SphereSweep)
	{
		FCollisionShape Shape;
		Shape.SetSphere(Range);

		TArray<FHitResult> HitResult;

		GetWorld()->SweepMultiByChannel(HitResult, MyPos, MyPos, FQuat::Identity, ECollisionChannel::ECC_Pawn, Shape);

		for(auto Curr : HitResult)
		{
			if(APawn* CurrPawn = Cast<APawn>(Curr.GetActor()))
			{
				if(DistanceFromPawn(CurrPawn) <= Range)
				{
					PawnsInRange.Add(CurrPawn);
				}
			}
		}
	}
	
	else if (DetectionMethod == EDetectionMethod::GetAllPawns)
	{
		TArray<AActor*> Actors;
		
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), Actors);

		for(auto* Curr : Actors)
		{
			if(APawn* CurrPawn = Cast<APawn>(Curr))
			{
				if(DistanceFromPawn(CurrPawn) <= Range)
				{
					PawnsInRange.Add(CurrPawn);
				}
			}
		}
	}
	
	return PawnsInRange;
}

float AEC_Turret::DistanceFromPawn(const APawn* Pawn) const
{
	float Dist = -1.0f;
	
	if(Pawn)
	{
		const FVector MyLoc = GetActorLocation();
		const FVector PawnLoc = Pawn->GetActorLocation();

		const float A = PawnLoc.X - MyLoc.X;
		const float B = PawnLoc.Y - MyLoc.Y;
		const float C = PawnLoc.Z - MyLoc.Z;

		const float Hypotenuse = FMath::Sqrt(FMath::Pow(A, 2) + FMath::Pow(B, 2) + FMath::Pow(C, 2));

		Dist = Hypotenuse;
	}

	return Dist;
}

void AEC_Turret::DrawDebug()
{
	if(bDrawDebug)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), Range, 32, FColor::Red);
		
		if(CurrentTarget)
		{
			DrawDebugLine(GetWorld(), GetStaticMeshComponent()->GetSocketLocation(ProjectileSpawnSocketName), CurrentTarget->GetActorLocation(),
			FColor::Red, false, -1, 0, 5.0f);
		}
		
	}
}

