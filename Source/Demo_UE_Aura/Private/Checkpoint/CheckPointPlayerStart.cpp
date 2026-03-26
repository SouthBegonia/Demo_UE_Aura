


#include "Checkpoint/CheckPointPlayerStart.h"
#include "AuraLogChannels.h"
#include "Components/SphereComponent.h"
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

}

void ACheckPointPlayerStart::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ACheckPointPlayerStart::OnSphereOverlap);
	SphereComp->OnComponentEndOverlap.AddDynamic(this, &ACheckPointPlayerStart::OnSphereEndOverlap);
}

void ACheckPointPlayerStart::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		LastReachedPlayer = OtherActor;

		// BP show effect
		CheckPointReached();

		// SaveGame
		if (PlayerStartTag.IsNone())
			UE_LOGFMT(LogAura, Warning, "[{FUNC}] : PlayerStartTag is empty. Obj={Log}", __FUNCTION__, GetName());
		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
	}
}

void ACheckPointPlayerStart::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		CheckPointFled();
	}
}
