


#include "UI/ViewModel/MVVM_VM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_VM_LoadSlot.h"

#define GAME_SLOT_NAME TEXT("AuraSaveData")

void UMVVM_VM_LoadScreen::InitializeLoadSlots()
{
	// Create LoadSlotVM
	LoadSlot_0 = NewObject<UMVVM_VM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1 = NewObject<UMVVM_VM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2 = NewObject<UMVVM_VM_LoadSlot>(this, LoadSlotViewModelClass);

	// Initialize LoadSlotVM info
	LoadSlot_0->InitializeSlot(false);
	LoadSlot_1->InitializeSlot(false);
	LoadSlot_2->InitializeSlot(false);
	LoadSlot_0->SetSlotIndex(0);
	LoadSlot_1->SetSlotIndex(1);
	LoadSlot_2->SetSlotIndex(2);
	LoadSlot_0->SetLoadSlotName(FString::Printf(TEXT("%s_%d"), GAME_SLOT_NAME, LoadSlot_0->GetSlotIndex()));
	LoadSlot_1->SetLoadSlotName(FString::Printf(TEXT("%s_%d"), GAME_SLOT_NAME, LoadSlot_1->GetSlotIndex()));
	LoadSlot_2->SetLoadSlotName(FString::Printf(TEXT("%s_%d"), GAME_SLOT_NAME, LoadSlot_2->GetSlotIndex()));


	LoadSlotsMap.Add(0, LoadSlot_0);
	LoadSlotsMap.Add(1, LoadSlot_1);
	LoadSlotsMap.Add(2, LoadSlot_2);
}

void UMVVM_VM_LoadScreen::LoadAllSlotData()
{
	// Load ULoadScreenSaveGame to initialize all LoadSlotVM
	for (const TPair<int32, UMVVM_VM_LoadSlot*>& Pair : LoadSlotsMap)
	{
		const int32 SlotIndex = Pair.Key;

		LoadTargetSlotData(SlotIndex);
	}
}

void UMVVM_VM_LoadScreen::LoadTargetSlotData(const int32 SlotIndex)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));

	// Load ULoadScreenSaveGame to initialize LoadSlotVM
	UMVVM_VM_LoadSlot* LoadSlot = GetLoadSlotViewModel(SlotIndex);
	const ULoadScreenSaveGame* SaveGameObject = AuraGameMode->GetTargetSaveSlotData(LoadSlot->GetLoadSlotName(), SlotIndex);

	LoadSlot->UpdateLoadSlotBySaveData(*SaveGameObject);
}

UMVVM_VM_LoadSlot* UMVVM_VM_LoadScreen::GetLoadSlotViewModel(int32 Index) const
{
	return LoadSlotsMap.FindChecked(Index);
}

void UMVVM_VM_LoadScreen::SlotButtonPressed_NewSlot(int32 SlotIndex, const FString& EnterName)
{
	// TODO : check EnterName is valid.
	if (EnterName.IsEmpty())
		return;

	ClearSlotSelection();

	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		UMVVM_VM_LoadSlot* LoadSlotVM = GetLoadSlotViewModel(SlotIndex);
		UMVVM_VM_LoadSlot* LoadSlotVM_Temp = DuplicateObject(LoadSlotVM, this);

		// set base info
		//		use LoadSlotVM_Temp info was just for create/initialize SaveGameData
		LoadSlotVM_Temp->SetPlayerName(EnterName);
		LoadSlotVM_Temp->SetPlayerLevel(1);
		LoadSlotVM_Temp->SetMapName(AuraGameMode->GetDefaultMapName());

		// Save Data
		if (AuraGameMode->SaveTargetSlotData(*LoadSlotVM_Temp, SlotIndex))
		{
			// Load SaveData (to update VM and UI)
			LoadTargetSlotData(SlotIndex);
		}
		LoadSlotVM_Temp = nullptr;
	}
}

void UMVVM_VM_LoadScreen::SlotButtonPressed_NewGame(int32 SlotIndex)
{
	ClearSlotSelection();

	UMVVM_VM_LoadSlot* LoadSlotVM = GetLoadSlotViewModel(SlotIndex);
	LoadSlotVM->SetSlotStatus(ESaveSlotStatus::EnterName, true);
}

void UMVVM_VM_LoadScreen::SlotButtonPressed_SelectSlot(int32 SlotIndex)
{
	SetSlotSelection(SlotIndex);
}

void UMVVM_VM_LoadScreen::SlotButtonPressed_Play()
{
	if (CurrentSelectLoadSlot.IsValid())
	{
		check(CurrentSelectSlotIndex != -1)

		if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			const UMVVM_VM_LoadSlot* CurrentLoadSlot = CurrentSelectLoadSlot.Get();
			AuraGameMode->TravelToMap(CurrentLoadSlot->GetMapName());
		}
	}
}

void UMVVM_VM_LoadScreen::SlotButtonPressed_DeleteSlot()
{
	if (CurrentSelectLoadSlot.IsValid())
	{
		check(CurrentSelectSlotIndex != -1)

		UMVVM_VM_LoadSlot* CurrentLoadSlot = CurrentSelectLoadSlot.Get();
		const FString SlotName = CurrentLoadSlot->GetLoadSlotName();

		if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			if (AuraGameMode->DeleteTargetSlotData(*CurrentLoadSlot, CurrentSelectSlotIndex))
			{
				// Initialize LoadSlotVM data
				CurrentLoadSlot->InitializeSlot(true);

				// Deselect slot
				ClearSlotSelection();

				UE_LOGFMT(LogTemp, Log, "[{FUNC}] : DeleteSlot success. SlotName = {SlotName}", __FUNCTION__, SlotName);
			}

		}
	}
}

void UMVVM_VM_LoadScreen::SetSlotSelection(int32 SlotIndex)
{
	// set all slots
	for (const TPair<int32, UMVVM_VM_LoadSlot*>& Pair : LoadSlotsMap)
	{
		const bool bSelected = Pair.Key == SlotIndex;
		Pair.Value->SetSelected(bSelected);
	}

	// cache data
	CurrentSelectLoadSlot = GetLoadSlotViewModel(SlotIndex);
	UE_MVVM_SET_PROPERTY_VALUE(CurrentSelectSlotIndex, SlotIndex);
	//AnySlotSelectedDelegate.Broadcast();
}

void UMVVM_VM_LoadScreen::ClearSlotSelection()
{
	// set all Slots
	for (const TPair<int32, UMVVM_VM_LoadSlot*>& Pair : LoadSlotsMap)
	{
		Pair.Value->SetSelected(false);
	}

	// reset data
	CurrentSelectLoadSlot = nullptr;
	UE_MVVM_SET_PROPERTY_VALUE(CurrentSelectSlotIndex, -1);
}
