

#pragma once

#include "CoreMinimal.h"
#include "Demo_UE_Aura/Demo_UE_Aura.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "CheckPointPlayerStart.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API ACheckPointPlayerStart : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	ACheckPointPlayerStart(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	virtual void OnActorLoaded_Implementation() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointReached();
	UFUNCTION(BlueprintImplementableEvent)
	void CheckPointFled();

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura")
	int32 CustomDepthStencilOverride = CUSTOM_DEPTH_TAN;

	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComp;


	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToSign;


	UPROPERTY()
	TWeakObjectPtr<AActor> LastReachedPlayer;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
