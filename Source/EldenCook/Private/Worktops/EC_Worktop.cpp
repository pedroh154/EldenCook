
#include "EldenCook/Public/Worktops/EC_Worktop.h"
#include "Components/BoxComponent.h"
#include "EldenCook/EldenCook.h"
#include "Player/EldenCookCharacter.h"
#include "Worktops/EC_IngredientSpawner.h"

// Sets default values
AEC_Worktop::AEC_Worktop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(MeshComponent);
	BoxComponent->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	BoxComponent->SetCollisionProfileName(TEXT("Interactable"));
	BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxComponent->SetCollisionResponseToChannel(COLLISION_INTERACTABLE, ECR_Overlap);
	//BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AEC_Worktop::OnBoxComponentBeginOverlap);
	//BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AEC_Worktop::OnBoxComponentEndOverlap);
}

// Called when the game starts or when spawned
void AEC_Worktop::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEC_Worktop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// void AEC_Worktop::OnBoxComponentBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
// 	if(AEldenCookCharacter* Char = Cast<AEldenCookCharacter>(OtherActor))
// 	{
// 		InteractingCharacters.Add(Char);
//
// 		if(Char->IsLocallyControlled())
// 			ToggleInteractingMaterial();
// 	}
// }
//
// void AEC_Worktop::OnBoxComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// 	if(AEldenCookCharacter* Char = Cast<AEldenCookCharacter>(OtherActor))
// 	{
// 		InteractingCharacters.Remove(Char);
//
// 		if(Char->IsLocallyControlled())
// 			ToggleInteractingMaterial();
// 	}
// }

bool AEC_Worktop::CanInteract(AEldenCookCharacter* InteractingChar)
{
	//bool bIsInsideBoxCollision = false;
	
	// for(int32 i = 0; i < InteractingCharacters.Num(); ++i)
	// {
	// 	bIsInsideBoxCollision = InteractingChar == InteractingCharacters[i];
	// 	if(bIsInsideBoxCollision) break;
	// }
	
	return IsValid(InteractingChar) && GetLocalRole() == ROLE_Authority;
}

void AEC_Worktop::OnInteract()
{
	
}

void AEC_Worktop::OnHighlighted(AEldenCookCharacter* InteractingChar)
{
	InteractingCharacters.Add(InteractingChar);
	SetInteractingMaterial();
}

void AEC_Worktop::OnUnhilighted(AEldenCookCharacter* InteractingChar)
{
	InteractingCharacters.Remove(InteractingChar);
	SetInteractingMaterial();
}

void AEC_Worktop::SetInteractingMaterial()
{
	const UStaticMesh* CurrMesh = MeshComponent->GetStaticMesh();
	
	if(IsValid(MaterialWhileInteracting) && IsValid(CurrMesh))
	{
		PreviousMaterial = CurrMesh->GetMaterial(0);
		MeshComponent->GetStaticMesh()->SetMaterial(0, MaterialWhileInteracting);
	}
}

void AEC_Worktop::RemoveInteractingMaterial()
{
	UStaticMesh* CurrMesh = MeshComponent->GetStaticMesh();
	
	if(IsValid(CurrMesh))
	{
		CurrMesh->SetMaterial(0, PreviousMaterial);
	}
}

