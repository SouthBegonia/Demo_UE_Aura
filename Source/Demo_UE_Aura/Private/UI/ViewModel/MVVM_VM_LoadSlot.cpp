


#include "UI/ViewModel/MVVM_VM_LoadSlot.h"

#include "AuraLogChannels.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"

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

FName UMVVM_VM_LoadSlot::GetPlayerStartTag() const
{
	if (!PlayerStartTag.IsNone())
		return PlayerStartTag;

	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	check(AuraGameMode)

	UE_LOGFMT(LogAura, Warning, "[{FUNC}] : PlayerStartTag from LoadSlot is empty, now will using GetDefaultPlayerStartTag={tag}", __FUNCTION__, AuraGameMode->GetDefaultPlayerStartTag().ToString());

	return AuraGameMode->GetDefaultPlayerStartTag();
}

void UMVVM_VM_LoadSlot::SetPlayerStartTag(const FName& NewTag)
{
	PlayerStartTag = NewTag;
}

void UMVVM_VM_LoadSlot::UpdateLoadSlotBySaveData(const ULoadScreenSaveGame& SaveGame)
{
	// TODO : check variable in SaveGame is valid. if variable is valid, should delete this SaveGame?

	SetPlayerName(SaveGame.PlayerName);
	SetPlayerLevel(SaveGame.PlayerLevel);
	SetMapName(SaveGame.MapName);

	SetPlayerStartTag(SaveGame.PlayerStartTag);

	SetSlotStatus(SaveGame.SaveSlotStatus, true);
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
