


#include "UI/ViewModel/MVVM_VM_LoadSlot.h"

void UMVVM_VM_LoadSlot::InitializeSlot(bool bNotify)
{
	// Initialize for new empty slot
	SetPlayerName(FString());
	SetPlayerLevel(0);

	SetSlotStatus(ESaveSlotStatus::Vacant, bNotify);
}

void UMVVM_VM_LoadSlot::SetSlotStatus(ESaveSlotStatus NewStatus, bool bNotifyChanged)
{
	const bool bNotify = bNotifyChanged;// && SlotStatus != NewStatus;

	SlotStatus = NewStatus;

	if (bNotify)
	{
		SetWidgetSwitcherIndexDelegate.Broadcast(SlotStatus.GetValue());
	}
}

void UMVVM_VM_LoadSlot::UpdateLoadSlotBySaveData(const ULoadScreenSaveGame& SaveGame)
{
	SetPlayerName(SaveGame.PlayerName);
	SetPlayerLevel(SaveGame.PlayerLevel);
	SetMapName(SaveGame.MapName);

	SetSlotStatus(SaveGame.SaveSlotStatus, true);;
}

void UMVVM_VM_LoadSlot::SetSelected(bool bSelected)
{
	SelectedSlotDelegate.Broadcast(bSelected);
}

#pragma region Field Notifies

void UMVVM_VM_LoadSlot::SetPlayerName(const FString& InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_VM_LoadSlot::SetPlayerLevel(const int32 InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}

void UMVVM_VM_LoadSlot::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_VM_LoadSlot::SetLoadSlotName(const FString& InName)
{
	check(LoadSlotName.IsEmpty())	// Only could call once on UMVVM_VM_LoadScreen::InitializeLoadSlots()
	UE_MVVM_SET_PROPERTY_VALUE(LoadSlotName, InName);
}

void UMVVM_VM_LoadSlot::SetSlotIndex(int32 InSlotIndex)
{
	check(SlotIndex == -1)	// Only could call once on UMVVM_VM_LoadScreen::InitializeLoadSlots()
	UE_MVVM_SET_PROPERTY_VALUE(SlotIndex, InSlotIndex);
}

#pragma endregion
