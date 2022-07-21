#pragma once
#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "EC_Turret.generated.h"

UENUM()
enum class EDetectionMethod
{
	GetAllPawns,
	SphereSweep
};

UENUM()
enum class ETurretState
{
	Idle,
	Shooting
};

UCLASS()
class ELDENCOOK_API AEC_Turret : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	AEC_Turret();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void DetermineCurrentTarget();
	virtual void DetermineTurretState();
	virtual void StartShooting();
	virtual void StopShooting();

private:
	virtual void Shoot();

public:
	TArray<APawn*> GetCharactersInRange() const;
	float DistanceFromPawn(const APawn* Pawn) const;

private:
	virtual void DrawDebug();

private:
	UPROPERTY(EditAnywhere, Category="AEC_Turret|Settings")
	bool bDrawDebug = false;

	UPROPERTY(EditAnywhere, Category="AEC_Turret|Settings")
	EDetectionMethod DetectionMethod = EDetectionMethod::GetAllPawns;

	FTimerHandle TimeBetweenShotsHandle;

protected:
	UPROPERTY()
	APawn* CurrentTarget;

	UPROPERTY(VisibleAnywhere, Category="AEC_Turret|Status")
	ETurretState State = ETurretState::Idle;
	
	/* characters within range are gonna get detected */
	UPROPERTY(EditAnywhere, Category="AEC_Turret|Settings")
	float Range = 300.0f;
	
	UPROPERTY(EditAnywhere, Category="AEC_Turret|Settings")
	FVector RotationSpeed = FVector::OneVector;

	/* how many seconds between each shot */
	UPROPERTY(EditAnywhere, Category="AEC_Turret|Settings")
	float ShotRate = 1.0f;

	/* how many seconds to wait before taking the first shot.
	 * if 0, will shoot as soon as CurrentTarget is set */
	UPROPERTY(EditAnywhere, Category="AEC_Turret|Settings")
	float FirstShotDelay = 1.0f;

	UPROPERTY(EditAnywhere, Category="AEC_Turret|Settings")
	FName ProjectileSpawnSocketName = TEXT("SOCKET_ProjectileSpawn");
};
