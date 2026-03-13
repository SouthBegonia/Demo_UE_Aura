

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartUpgoingTimeline();

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> ReturnToActor;

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageEffectParams;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	TSet<TWeakObjectPtr<AActor>> SphereOverlappedActors;
};
