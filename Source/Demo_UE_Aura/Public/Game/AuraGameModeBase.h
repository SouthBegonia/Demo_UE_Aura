

#pragma once

#include "CoreMinimal.h"
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

public:
	UPROPERTY(EditDefaultsOnly, Category="Aura|Character Class Default")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category="Aura|Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

#pragma region SaveGame

public:
	bool SaveTargetSlotData(const UMVVM_VM_LoadSlot& LoadSlot, int32 SlotIndex) const;
	ULoadScreenSaveGame* GetTargetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	bool DeleteTargetSlotData(const UMVVM_VM_LoadSlot& LoadSlot, int32 SlotIndex) const;

	UPROPERTY(EditDefaultsOnly, Category="Aura|SaveGame")
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

private:
	ULoadScreenSaveGame* CreateSlotSaveObject(const UMVVM_VM_LoadSlot* LoadSlot = nullptr) const;

#pragma endregion
};
