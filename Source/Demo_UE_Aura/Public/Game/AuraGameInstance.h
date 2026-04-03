

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
	/* call when enter game success by using any SaveGame */
	void MarkLoadedGameSave() { bLoadedGameSave = true; }
	/* call when restart game or restart another SaveGame */
	void ResetLoadedGameSaveDoneMark() { bLoadedGameSave = false; ClearInitializedPlayerInfoFromGameSaveDoneMark(); }
	/* check is recorded necessary info witch set from SaveGame this time */
	bool IsLoadedGameSave() const { return bLoadedGameSave; }

	/* call when initialized PlayerInfo by using any SaveGame or UAuraGameInstance data */
	void MarkInitializedPlayerInfoFromGameSave() { bInitializedPlayerInfoFromGameSave = true; }
	/* call when restart game or restart another SaveGame */
	void ClearInitializedPlayerInfoFromGameSaveDoneMark() { bInitializedPlayerInfoFromGameSave = false; }
	/* check is initialized necessary PlayerInfo */
	bool IsInitializedPlayerInfoFromGameSave() const { return bInitializedPlayerInfoFromGameSave; }


	/**
	 * A PlayerStartTag, player will spawn on the target PlayerStartActor when enter a map
	 */
	UPROPERTY()
	FName PlayerStartTag;

	FString CurrentMapName;

	UPROPERTY()
	FString LoadSlotName = FString();

	UPROPERTY()
	int32 LoadSlotIndex = 0;

private:
	bool bLoadedGameSave = false;
	bool bInitializedPlayerInfoFromGameSave = false;

#pragma endregion
};
