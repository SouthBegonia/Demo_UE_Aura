

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemy;
class AAuraEnemySpawnPoint;
class UBoxComponent;


/**
 * Spawn Enemies when Player Enter Volume
 */
UCLASS()
class DEMO_UE_AURA_API AAuraEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()
	
public:
	AAuraEnemySpawnVolume();


	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AAuraEnemySpawnPoint>> SpawnPoints;

public:
	virtual void OnActorLoaded_Implementation() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxVolume;
};
