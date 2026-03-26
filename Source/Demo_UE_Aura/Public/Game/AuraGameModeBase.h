

#pragma once

#include "CoreMinimal.h"
#include "LoadScreenSaveGame.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

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


	void TravelToMap(const FString& MapName);

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

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

	UPROPERTY(EditDefaultsOnly, Category="Aura|SaveGame")
	TMap<FString, TSoftObjectPtr<UWorld>> AllMaps;

private:
	void InitMapsInfo();

#pragma endregion

#pragma region SaveGame (For InGame)

public:
	ULoadScreenSaveGame* RetrieveInGameSaveData();

	bool ModifyInGameSaveData(ULoadScreenSaveGame& SaveData, FSaveGameModifiableParams& ModifyParams);

	bool SaveInGameProgressData(ULoadScreenSaveGame& SaveData, const FString& SlotName, int32 SlotIndex);

#pragma endregion
};
