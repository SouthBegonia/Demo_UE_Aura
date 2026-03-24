


#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_VM_LoadSlot.h"

#pragma region SaveGame

bool AAuraGameModeBase::SaveTargetSlotData(const UMVVM_VM_LoadSlot& LoadSlot, int32 SlotIndex) const
{
	// Delete old data
	if (UGameplayStatics::DoesSaveGameExist(LoadSlot.GetLoadSlotName(), SlotIndex))
		UGameplayStatics::DeleteGameInSlot(LoadSlot.GetLoadSlotName(), SlotIndex);

	// Create a new SaveGameData
	ULoadScreenSaveGame* LoadScreenSaveGame = CreateSlotSaveObject(&LoadSlot);
	LoadScreenSaveGame->SaveSlotStatus = Taken;

	// Save data to local
	const bool bSaveSuccess = UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot.GetLoadSlotName(), SlotIndex);
	if (!bSaveSuccess)
		UE_LOGFMT(LogTemp, Error, "[{FUNC}] : SaveGameToSlot failed: SlotName={SlotName}, SlotIndex={SlotIndex}", __FUNCTION__, LoadSlot.GetLoadSlotName(), SlotIndex);

	return bSaveSuccess;
}

bool AAuraGameModeBase::DeleteTargetSlotData(const UMVVM_VM_LoadSlot& LoadSlot, int32 SlotIndex) const
{
	bool bDeleteSuccess = true;

	if (UGameplayStatics::DoesSaveGameExist(LoadSlot.GetLoadSlotName(), SlotIndex))
	{
		bDeleteSuccess = UGameplayStatics::DeleteGameInSlot(LoadSlot.GetLoadSlotName(), SlotIndex);
		if (!bDeleteSuccess)
			UE_LOGFMT(LogTemp, Error, "[{FUNC}] : DeleteGameSlot failed: SlotName={SlotName}, SlotIndex={SlotIndex}", __FUNCTION__, LoadSlot.GetLoadSlotName(), SlotIndex);
	}

	return bDeleteSuccess;
}

ULoadScreenSaveGame* AAuraGameModeBase::GetTargetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;

	// Try LoadSaveData or Create a new one
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	else
		SaveGameObject = CreateSlotSaveObject(nullptr);

	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

ULoadScreenSaveGame* AAuraGameModeBase::CreateSlotSaveObject(const UMVVM_VM_LoadSlot* LoadSlot) const
{
	check(LoadScreenSaveGameClass)
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);

	if (LoadSlot != nullptr)
	{
		LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	}

	return LoadScreenSaveGame;
}

#pragma endregion
