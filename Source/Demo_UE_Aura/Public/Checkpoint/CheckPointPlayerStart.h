

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CheckPointPlayerStart.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API ACheckPointPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	ACheckPointPlayerStart(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached();
	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointFled();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComp;



	UPROPERTY()
	TWeakObjectPtr<AActor> LastReachedPlayer;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
