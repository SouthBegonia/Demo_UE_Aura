


#include "Checkpoint/CheckPointPlayerStart.h"
#include "AuraLogChannels.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Demo_UE_Aura/Demo_UE_Aura.h"
#include "Interaction/PlayerInterface.h"

ACheckPointPlayerStart::ACheckPointPlayerStart(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(CheckpointMesh);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToSign = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToSign"));
	MoveToSign->SetupAttachment(GetRootComponent());

	CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
}

void ACheckPointPlayerStart::OnActorLoaded_Implementation()
{
	UE_LOGFMT(LogTemp, Log, "[{FUNC}] : {Log}", __FUNCTION__,  TEXT("11111"));

	// Do this in BeginPlay
	if (bReached)
	{
		// BP show effect
		//CheckPointReached();

		// Limit SavingGame by bReached in ACheckPointPlayerStart::OnSphereOverlap(), no need for set Collision
		//	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ACheckPointPlayerStart::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACheckPointPlayerStart::OnSphereOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ACheckPointPlayerStart::OnSphereEndOverlap);

	if (bReached)
	{
		// BP show effect
		CheckPointReached();

		// Limit SavingGame by bReached in ACheckPointPlayerStart::OnSphereOverlap(), no need for set Collision
		//	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ACheckPointPlayerStart::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		//if (bReached) return;	// maybe we shouldn't limit the right of SavingGame

		LastReachedPlayer = OtherActor;

		// BP show effect
		CheckPointReached();

		// SaveGame
		if (PlayerStartTag.IsNone())
			UE_LOGFMT(LogAura, Warning, "[{FUNC}] : PlayerStartTag is empty. Obj={Log}", __FUNCTION__, GetName());
		bReached = true;	// set value before Save it
		bool bSavedSuccessful = IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		if (bSavedSuccessful)
		{
			bReached = true;
			UE_LOGFMT(LogAura, Log, "[{FUNC}] : SaveGame successful.", __FUNCTION__);
		}
		else
		{
			bReached = false;
			UE_LOGFMT(LogAura, Error, "[{FUNC}] : SaveGame failed.", __FUNCTION__);
		}
	}
}

void ACheckPointPlayerStart::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		if (bReached)
			return;

		CheckPointFled();
	}
}


void ACheckPointPlayerStart::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
	//CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
}

void ACheckPointPlayerStart::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void ACheckPointPlayerStart::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToSign->GetComponentLocation();
}