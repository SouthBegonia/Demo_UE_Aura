

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()

#pragma region Save/LoadGame

public:
	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 LoadSlotIndex = 0;

#pragma endregion
};
