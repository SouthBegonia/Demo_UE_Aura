


#include "Game/AuraGameModeBase.h"

#include "AuraLogChannels.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
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

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllActors);

	if (AllActors.Num() > 0)
	{
		AActor* SelectedActor = AllActors[0];
		for (AActor* const& Ac : AllActors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Ac))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					return SelectedActor;
				}
			}
		}

		UE_LOGFMT(LogAura, Warning, "[{FUNC}] : could not find a PlayerStartActor witch PlayerStartTag = [{Log}] in world.", __FUNCTION__, AuraGameInstance->PlayerStartTag);
		return SelectedActor;
	}

	UE_LOGFMT(LogAura, Error, "[{FUNC}] : there aren't any PlayerStartActor in world.", __FUNCTION__);
	return nullptr;
}

#pragma region SaveGame ((For LoadMenu)

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

ULoadScreenSaveGame* AAuraGameModeBase::GetTargetSaveSlotData(const FString& SlotName, int32 SlotIndex, const bool bCreateWhenNull) const
{
	USaveGame* SaveGameObject = nullptr;

	// Try LoadSaveData or Create a new one
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);

	if (SaveGameObject == nullptr && bCreateWhenNull)
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
		LoadScreenSaveGame->PlayerStartTag = LoadSlot->GetPlayerStartTag();
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

#pragma region SaveGame (For InGame)

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData()
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString InGameLoadSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameLoadSlotIndex = AuraGameInstance->LoadSlotIndex;

	ULoadScreenSaveGame* InGameSaveSlotData = GetTargetSaveSlotData(InGameLoadSlotName, InGameLoadSlotIndex, false);
	if (InGameSaveSlotData == nullptr)
	{
		UE_LOGFMT(LogAura, Error, "[{FUNC}] : Can't get SlotData. InGameLoadSlotName={Name}, InGameLoadSlotIndex={Index}", __FUNCTION__, InGameLoadSlotName, InGameLoadSlotIndex);
		// this shouldn't happen, there are possible situations:
		//		- Delete the SaveData when playing
	}

	return InGameSaveSlotData;
}

bool AAuraGameModeBase::ModifyInGameSaveData(ULoadScreenSaveGame& SaveData, FSaveGameModifiableParams& ModifyParams)
{
	SaveData.PlayerStartTag = ModifyParams.PlayerStartTag;

	return true;
}

bool AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame& SaveData, const FString& SlotName, int32 SlotIndex)
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UE_LOGFMT(LogAura, Error, "[{FUNC}] : Can't get SlotData. SlotName={Name}, SlotIndex={Index}", __FUNCTION__, SlotName, SlotIndex);
		// this shouldn't happen, there are possible situations:
		//		- Delete the SaveData when playing
	}

	// Save data to local
	const bool bSaveSuccess = UGameplayStatics::SaveGameToSlot(&SaveData, SlotName, SlotIndex);
	if (!bSaveSuccess)
		UE_LOGFMT(LogTemp, Error, "[{FUNC}] : SaveGameToSlot failed: SlotName={SlotName}, SlotIndex={SlotIndex}", __FUNCTION__, SlotName, SlotIndex);

	return bSaveSuccess;
}


#pragma endregion
