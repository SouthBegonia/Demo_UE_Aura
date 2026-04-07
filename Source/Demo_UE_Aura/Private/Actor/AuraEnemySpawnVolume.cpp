


#include "Actor/AuraEnemySpawnVolume.h"

#include "Actor/AuraEnemySpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Interaction/PlayerInterface.h"

// Sets default values
AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxVolume"));
	SetRootComponent(BoxVolume);
	BoxVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxVolume->SetCollisionObjectType(ECC_WorldStatic);
	BoxVolume->SetCollisionResponseToChannels(ECR_Ignore);
	BoxVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	// There might be more than one AAuraEnemySpawnVolume in the World. Now we should add item in Level to SpawnPoints in Editor
	//TArray<AActor*> AllActors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAuraEnemySpawnPoint::StaticClass(), AllActors);
	//for (AActor* const& Actor : AllActors)
	//	SpawnPoints.AddUnique(Cast<AAuraEnemySpawnPoint>(Actor));
	check(SpawnPoints.Num() > 0);


	BoxVolume->OnComponentBeginOverlap.AddDynamic(this, &AAuraEnemySpawnVolume::OnSphereOverlap);
}

void AAuraEnemySpawnVolume::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	//if (bReached) return;

	if (!OtherActor->Implements<UPlayerInterface>())
		return;

	bReached = true;
	BoxVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	for (const TObjectPtr<AAuraEnemySpawnPoint>& SpawnPoint : SpawnPoints)
	{
		if (IsValid(SpawnPoint))
		{
			AAuraEnemy* Enemy = SpawnPoint->SpawnEnemy();
		}
	}
}

void AAuraEnemySpawnVolume::OnActorLoaded_Implementation()
{
	if (bReached)
		Destroy();
	// TODO : could destroy after all enemies died ?
}