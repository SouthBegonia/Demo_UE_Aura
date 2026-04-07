// 

#pragma once

#include "CoreMinimal.h"
#include "CheckPointPlayerStart.h"
#include "MapEntrance.generated.h"

UCLASS()
class DEMO_UE_AURA_API AMapEntrance : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMapEntrance(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditDefaultsOnly, Category="Aura", meta=(ToolTip="Highlight CheckpointMesh if enabled"))
	bool bEnableHighlighting = true;


	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;


	UPROPERTY(EditAnywhere, Category="Aura", DisplayName="DestinationMapAsset", meta=(ToolTip="The Map Player will travel to", NoResetToDefault))
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere, Category="Aura", DisplayName="PlayerStartTagInDestinationMap", meta=(ToolTip="Player will travel to DestinationMap, and spawn in PlayerStartActor witch APlayerStart::PlayerStartTag equal with DestinationPlayerStartTag", NoResetToDefault))
	FName DestinationPlayerStartTag;

	UPROPERTY(EditAnywhere, Category="Aura", DisplayName="PlayerStartTag Of Entrance", meta=(ToolTip="When Player travel from other entrance, it will spawn in PlayerStartActor witch APlayerStart::PlayerStartTag equal with DestinationPlayerStartTag", NoResetToDefault))
	FName EntrancePlayerStartTag;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> DisplayAttachRoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USphereComponent> SphereComp;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> MoveToSign;

private:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	virtual void OnActorLoaded_Implementation() override;
};
