

#pragma once

#include "CoreMinimal.h"
#include "LoadScreenSaveGame.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULootTiers;
class ULoadScreenSaveGame;
class USaveGame;
class UMVVM_VM_LoadSlot;
class UAbilityInfo;
class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Character Class Default")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

	/**
	 *
	 * @param MapName The name of target map, not the asset name
	 * @param PlayerStartTagInTargetMap The PlayerStartTag in target map. It will spawn player at the PlayerStart witch PlayerStartTag same with this
	 */
	void TravelToMap(const FString& MapName, FName PlayerStartTagInTargetMap = FName());

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void PlayerDied(ACharacter* DeadCharacter);

#pragma region SaveGame

private:
	bool FinalSaveGameToLocal(USaveGame* SaveGameObject, const FString& SlotName, const int32 SlotIndex) const;

	USaveGame* FinalLoadGameFromLocal(const FString& SlotName, const int32 SlotIndex, bool bCheckNull = true) const;

#pragma endregion

#pragma region SaveGame (For LoadMenu)

public:
	bool SaveTargetSlotData(const UMVVM_VM_LoadSlot& LoadSlot, int32 SlotIndex) const;
	ULoadScreenSaveGame* GetTargetSaveSlotData(const FString& SlotName, int32 SlotIndex, bool bCreateWhenNull) const;
	bool DeleteTargetSlotData(const UMVVM_VM_LoadSlot& LoadSlot, int32 SlotIndex) const;

	UPROPERTY(EditDefaultsOnly, Category="Aura|SaveGame")
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

private:
	ULoadScreenSaveGame* CreateSlotSaveObject(const UMVVM_VM_LoadSlot* LoadSlot = nullptr) const;


public:
	FString GetDefaultMapName() const { return DefaultMapName; }
	FName GetDefaultPlayerStartTag() const { return DefaultPlayerStartTag; }

	TSoftObjectPtr<UWorld> GetMapByName(const FString& InMapName) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|SaveGame")
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly, Category="Aura|SaveGame")
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly, Category="Aura|SaveGame")
	FName DefaultPlayerStartTag;

	/**
	 * Key = MapName, Value = Map
	 */
	UPROPERTY(EditDefaultsOnly, Category="Aura|SaveGame")
	TMap<FString, TSoftObjectPtr<UWorld>> AllMaps;

private:
	void InitMapsInfo();

#pragma endregion

#pragma region SaveGame (For InGame)

public:
	ULoadScreenSaveGame* RetrieveInGameSaveData();

	bool ModifyInGameSaveData(ULoadScreenSaveGame& SaveData, FSaveGameModifiableParams& ModifyParams, APlayerState* PlayerState);

	bool SaveInGameProgressData(ULoadScreenSaveGame& SaveData, const FString& SlotName, int32 SlotIndex) const;

	bool LoadWorldStateWithSaveGame(UWorld* InWorld, ULoadScreenSaveGame* SaveData = nullptr);

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;
	FString GetMapAssetNameFromMapName(const FString& MapName) const;

private:
	void ModifyInGameSaveData_WorldState(ULoadScreenSaveGame& SaveData, UWorld* InWorld, const FString& InDestinationMapName = FString(""));

#pragma endregion
};
