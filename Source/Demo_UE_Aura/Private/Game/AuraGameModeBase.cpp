


#include "Game/AuraGameModeBase.h"

#include "Game/LoadScreenSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_VM_LoadSlot.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	InitMapsInfo();
}

void AAuraGameModeBase::TravelToMap(const FString& MapName)
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, AllMaps.FindChecked(MapName));
}

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
		// here to set SaveData param
		LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
		LoadScreenSaveGame->PlayerLevel = LoadSlot->GetPlayerLevel();
		LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	}

	return LoadScreenSaveGame;
}


void AAuraGameModeBase::InitMapsInfo()
{
	check(!DefaultMapName.IsEmpty() && !DefaultMap.IsNull())

	AllMaps.Add(DefaultMapName, DefaultMap);
}

TSoftObjectPtr<UWorld> AAuraGameModeBase::GetMapByName(const FString& InMapName) const
{
	TSoftObjectPtr<UWorld> TargetMap = nullptr;
	if (AllMaps.Contains(InMapName))
		TargetMap = AllMaps[InMapName];
	else
		UE_LOGFMT(LogTemp, Error, "[{FUNC}] : MapName = [{MapName}] is invalid. ", __FUNCTION__, InMapName);

	return TargetMap;
}

#pragma endregion
