

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

UCLASS()
class DEMO_UE_AURA_API AMagicCircle : public AActor
{
	GENERATED_BODY()
	
public:
	AMagicCircle();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecalComp;
};
