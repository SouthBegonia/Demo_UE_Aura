//


#include "Checkpoint/MapEntrance.h"

#include "AuraLogChannels.h"
#include "Components/SphereComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AMapEntrance::AMapEntrance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	DisplayAttachRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DisplayRoot"));
	DisplayAttachRoot->SetupAttachment(GetRootComponent());

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	CheckpointMesh->SetupAttachment(DisplayAttachRoot);
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	MoveToSign = CreateDefaultSubobject<USceneComponent>(TEXT("MoveToSign"));
	MoveToSign->SetupAttachment(DisplayAttachRoot);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(MoveToSign);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	if (bEnableHighlighting)
		CheckpointMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
}

void AMapEntrance::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Force Setting PlayerStartTag equal with EntrancePlayerStartTag
	if (!EntrancePlayerStartTag.IsNone())
		PlayerStartTag = EntrancePlayerStartTag;
}

// Called when the game starts or when spawned
void AMapEntrance::BeginPlay()
{
	Super::BeginPlay();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMapEntrance::OnSphereOverlap);
}

void AMapEntrance::HighlightActor_Implementation()
{
	if (!bEnableHighlighting) return;
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AMapEntrance::UnHighlightActor_Implementation()
{
	if (!bEnableHighlighting) return;
	CheckpointMesh->SetRenderCustomDepth(false);
}

void AMapEntrance::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToSign->GetComponentLocation();
}

void AMapEntrance::OnActorLoaded_Implementation()
{
	// Do nothing when loading a Map Entrance
}

void AMapEntrance::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName("Player")))
	{
		check(!DestinationPlayerStartTag.IsNone())
		check(!DestinationMap.IsNull())

		if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
			{
				// Travel to destination Map
				const FString MapName = AuraGameMode->GetMapNameFromMapAssetName(DestinationMap.GetAssetName());
				FSaveGameModifiableParams ModifyParams;
				ModifyParams.PlayerStartTag = DestinationPlayerStartTag;
				ModifyParams.MapName = MapName;

				// Plan_A : SaveGame (will setting PlayerStartTag internally
				//bool bSavedSuccessful = true;
				//bSavedSuccessful = IPlayerInterface::Execute_SaveProgressWithParams(OtherActor, ModifyParams); // TODO : maybe SaveGame after travel map is safer?
				//if (bSavedSuccessful)
				//	AuraGameMode->TravelToMap(DestinationMap.GetAssetName(), DestinationPlayerStartTag);

				// Plan_B : Just setting PlayerStartTag
				AuraGameMode->TravelToMap(MapName, DestinationPlayerStartTag);
			}
		}
	}
}