

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "MVVM_VM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndexSignature, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlotSelectedSignature, bool, bSelected);

/**
 * 
 */
UCLASS()
class DEMO_UE_AURA_API UMVVM_VM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeSlot(bool bNotify = false);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ESaveSlotStatus GetSlotStatus() const { return SlotStatus; }


	void SetSlotStatus(ESaveSlotStatus NewStatus, bool bNotifyChanged = false);

	void UpdateLoadSlotBySaveData(const ULoadScreenSaveGame& SaveGame);

	void SetSelected(bool bSelected);

private:
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus;


	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndexSignature SetWidgetSwitcherIndexDelegate;

	UPROPERTY(BlueprintAssignable)
	FSlotSelectedSignature SelectedSlotDelegate;

#pragma region Field Notifies

public:
	void SetPlayerName(const FString& InPlayerName);
	FString GetPlayerName() const { return PlayerName; }

	void SetPlayerLevel(const int32 InPlayerLevel);
	int32 GetPlayerLevel() const { return PlayerLevel; }


	void SetLoadSlotName(const FString& InName);
	FString GetLoadSlotName() const { return LoadSlotName; }
	void SetSlotIndex(int32 InSlotIndex);
	int32 GetSlotIndex() const { return SlotIndex; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	int32 PlayerLevel;

	// TODO : add Player Level, MapName


	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString LoadSlotName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	int32 SlotIndex = -1;

#pragma endregion
};
