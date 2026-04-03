


#include "Game/AuraGameModeBase.h"

#include "AuraLogChannels.h"
#include "EngineUtils.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_VM_LoadSlot.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	InitMapsInfo();
}

void AAuraGameModeBase::TravelToMap(const FString& MapName, FName PlayerStartTagInTargetMap)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	if (AuraGameInstance)
	{
		AuraGameInstance->CurrentMapName = MapName;

		if (!PlayerStartTagInTargetMap.IsNone())
			AuraGameInstance->PlayerStartTag = PlayerStartTagInTargetMap;
	}

	// Travel map
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

bool AAuraGameModeBase::FinalSaveGameToLocal(USaveGame* SaveGameObject, const FString& SlotName, const int32 SlotIndex) const
{
	// Save data to local
	const bool bSaveSuccess = UGameplayStatics::SaveGameToSlot(SaveGameObject, SlotName, SlotIndex);
	if (!bSaveSuccess)
		UE_LOGFMT(LogAura_SaveGame, Error, "[{FUNC}] : SaveGameToSlot failed: SlotName={SlotName}, SlotIndex={SlotIndex}", __FUNCTION__, SlotName, SlotIndex);
	else
		UE_LOGFMT(LogAura_SaveGame, Log, "[{FUNC}] : SaveGameToSlot successful: SlotName={SlotName}, SlotIndex={SlotIndex}", __FUNCTION__, SlotName, SlotIndex);

	//Cast<ULoadScreenSaveGame>(SaveGameObject)->PrintDebugInfo();

	return bSaveSuccess;
}

USaveGame* AAuraGameModeBase::FinalLoadGameFromLocal(const FString& SlotName, const int32 SlotIndex, bool bCheckNull) const
{
	// Load Save data from local
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		if (bCheckNull)
			UE_LOGFMT(LogAura_SaveGame, Log, "[{FUNC}] : Target Slot does not exist. SlotName=[{SlotName}], SlotIndex=[{SlotIndex}]", __FUNCTION__, SlotName, SlotIndex);

		return nullptr;
	}

	const USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	if (SaveGame == nullptr && bCheckNull)
		UE_LOGFMT(LogAura_SaveGame, Log, "[{FUNC}] : Load target Slot failed. SlotName=[{SlotName}], SlotIndex=[{SlotIndex}]", __FUNCTION__, SlotName, SlotIndex);

	return const_cast<USaveGame*>(SaveGame);
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

	// Save data
	return FinalSaveGameToLocal(LoadScreenSaveGame, LoadSlot.GetLoadSlotName(), SlotIndex);
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
	// Try LoadSaveData or Create a new one
	USaveGame* SaveGameObject = FinalLoadGameFromLocal(SlotName, SlotIndex, false);
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
		/*
		 *  Here to set SaveData param (for InGame params)
		 *		Another code is @see AAuraGameModeBase::ModifyInGameSaveData
		 */
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
		UE_LOGFMT(LogAura_SaveGame, Error, "[{FUNC}] : Can't get SlotData. InGameLoadSlotName={Name}, InGameLoadSlotIndex={Index}", __FUNCTION__, InGameLoadSlotName, InGameLoadSlotIndex);
		// this shouldn't happen, there are possible situations:
		//		- Delete the SaveData when playing
	}

	return InGameSaveSlotData;
}

bool AAuraGameModeBase::ModifyInGameSaveData(ULoadScreenSaveGame& SaveData, FSaveGameModifiableParams& ModifyParams, APlayerState* PlayerState)
{
	/*
	 *  Here to set SaveData param (for InGame params)
	 *		Another code is @see AAuraGameModeBase::CreateSlotSaveObject
	 */

	SaveData.PlayerStartTag = ModifyParams.PlayerStartTag;

	if (const AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(PlayerState))
	{
		UAuraAbilitySystemComponent* const AuraASC = Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent());

		// PlayerInfo
		SaveData.PlayerLevel = AuraPlayerState->GetPlayerLevel();
		SaveData.PlayerEXP = AuraPlayerState->GetPlayerEXP();
		SaveData.SpellPoints = AuraPlayerState->GetSpellPoints();
		SaveData.AttributePoints = AuraPlayerState->GetAttributePoints();
		UE_LOGFMT(LogAura_SaveGame, Log, "[SaveGame]-[{FUNC}] : PlayerInfo Done.", __FUNCTION__);

		// AttributeInfo
		const UAttributeSet* AttributeSet = AuraPlayerState->GetAttributeSet();
		SaveData.AS_Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(AttributeSet);
		SaveData.AS_Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(AttributeSet);
		SaveData.AS_Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(AttributeSet);
		SaveData.AS_Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(AttributeSet);
		UE_LOGFMT(LogAura_SaveGame, Log, "[SaveGame]-[{FUNC}] : AttributeInfo Done.", __FUNCTION__);

		// Abilities
		SaveData.SavedAbilities.Empty();
		if (HasAuthority())
		{
			FForEachAbility SavedAbilityDelegate;
			SavedAbilityDelegate.BindLambda([this, &AuraASC, &SaveData](const FGameplayAbilitySpec& AbilitySpec)
			{
				const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFromSpec(AbilitySpec);
				const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
				const FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

				FSavedAbility SavedAbility;
				SavedAbility.GameplayAbilityClass = Info.AbilityClass;
				SavedAbility.AbilityTag = Info.AbilityTag;
				SavedAbility.AbilityTypeTag = Info.AbilityTypeTag;
				SavedAbility.AbilityLevel = AbilitySpec.Level;
				SavedAbility.AbilityInputTag = AuraASC->GetInputTagFromSpec(AbilitySpec);
				SavedAbility.AbilityStatusTag = AuraASC->GetStatusFromSpec(AbilitySpec);

				SaveData.SavedAbilities.AddUnique(SavedAbility);
			});
			AuraASC->ForEachAbility(SavedAbilityDelegate);
			UE_LOGFMT(LogAura_SaveGame, Log, "[SaveGame]-[{FUNC}] : Abilities Done.", __FUNCTION__);
		}

		// World/Map
		ModifyInGameSaveData_WorldState(SaveData, GetWorld(), ModifyParams.MapName);

		SaveData.bIsFirstTimeLoadIn = false;
	}
	else
		UE_LOGFMT(LogAura_SaveGame, Error, "[{FUNC}] : PlayerState is nullptr", __FUNCTION__);

	return true;
}

bool AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame& SaveData, const FString& SlotName, int32 SlotIndex) const
{
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UE_LOGFMT(LogAura_SaveGame, Error, "[{FUNC}] : Can't get SlotData. SlotName={Name}, SlotIndex={Index}", __FUNCTION__, SlotName, SlotIndex);
		// this shouldn't happen, there are possible situations:
		//		- Delete the SaveData when playing
	}

	// Save data
	return FinalSaveGameToLocal(&SaveData, SlotName, SlotIndex);;
}

void AAuraGameModeBase::ModifyInGameSaveData_WorldState(ULoadScreenSaveGame& SaveData, UWorld* InWorld, const FString& InDestinationMapName)
{
	FString WorldName = InWorld->GetMapName();
	WorldName.RemoveFromStart(InWorld->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(InWorld->GetGameInstance());
	check(AuraGameInstance);


	if (!SaveData.HasMap(WorldName))
	{
		FSavedMap NewSavedMap;
		NewSavedMap.MapAssetName = WorldName;

		SaveData.SavedMaps.Add(NewSavedMap);
	}


	FSavedMap SavedMap = SaveData.GetSavedMapWithMapName(WorldName);

	// SavedActors
	SavedMap.SavedActors.Empty();
	for (FActorIterator ActorIterator(InWorld); ActorIterator; ++ActorIterator)
	{
		AActor* ItActor = *ActorIterator;
		if (!IsValid(ItActor) || !ItActor->Implements<USaveInterface>())
			continue;

		FSavedActor SavedActor;
		SavedActor.ActorName = ItActor->GetFName();
		SavedActor.ActorTransform = ItActor->GetActorTransform();

		FMemoryWriter MemoryWriter(SavedActor.Bytes);
		FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
		Archive.ArIsSaveGame = true;

		ItActor->Serialize(Archive);

		SavedMap.SavedActors.AddUnique(SavedActor);
	}

	// SavedMaps
	for (FSavedMap& MapToReplace : SaveData.SavedMaps)
	{
		if (MapToReplace.MapAssetName == WorldName)
			MapToReplace = SavedMap;
	}


	// Map AssetName
	if (InDestinationMapName != FString(""))
	{
		SaveData.MapAssetName = GetMapAssetNameFromMapName(InDestinationMapName);
		SaveData.MapName = InDestinationMapName;
	}

	UE_LOGFMT(LogAura_SaveGame, Log, "[SaveGame]-[{FUNC}] : WorldState Done.", __FUNCTION__);
}

bool AAuraGameModeBase::LoadWorldStateWithSaveGame(UWorld* InWorld, ULoadScreenSaveGame* SaveData)
{
	FString WorldName = InWorld->GetMapName();
	WorldName.RemoveFromStart(InWorld->StreamingLevelsPrefix);

	bool bLoadSuccessful = false;

	if (SaveData == nullptr) SaveData = RetrieveInGameSaveData();
	if (SaveData)
	{
		for (FActorIterator ActorIterator(InWorld); ActorIterator; ++ActorIterator)
		{
			AActor* ItActor = *ActorIterator;
			if (!IsValid(ItActor) || !ItActor->Implements<USaveInterface>())
				continue;

			for (FSavedActor& SavedActor : SaveData->GetSavedMapWithMapName(WorldName).SavedActors)
			{
				if (SavedActor.ActorName == ItActor->GetFName())
				{
					// Recovery data in Actor by SaveGame data
					if (ISaveInterface::Execute_ShouldLoadTransform(ItActor))
					{
						ItActor->SetActorTransform(SavedActor.ActorTransform);
					}


					FMemoryReader MemoryReader(SavedActor.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					ItActor->Serialize(Archive);	// converts binary bytes back into variables

					ISaveInterface::Execute_OnActorLoaded(ItActor);
				}
			}
		}
		bLoadSuccessful = true;
	}

	return bLoadSuccessful;
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (const TPair<FString, TSoftObjectPtr<UWorld>>& Pair : AllMaps)
	{
		const FString& MapName = Pair.Key;
		const TSoftObjectPtr<UWorld>& Map = Pair.Value;

		if (Map.ToSoftObjectPath().GetAssetName() == MapAssetName)
			return MapName;
	}

	UE_LOGFMT(LogAura_SaveGame, Error, "[{FUNC}] : not find MapName with MapAssetName = {Log}", __FUNCTION__, MapAssetName);
	return FString("");
}

FString AAuraGameModeBase::GetMapAssetNameFromMapName(const FString& MapName) const
{
	for (const TPair<FString, TSoftObjectPtr<UWorld>>& Pair : AllMaps)
	{
		if (Pair.Key == MapName)
			return Pair.Value.ToSoftObjectPath().GetAssetName();
	}

	UE_LOGFMT(LogAura_SaveGame, Error, "[{FUNC}] : not find MapAssetName with MapName = {Log}", __FUNCTION__, MapName);
	return FString("");
}


#pragma endregion
