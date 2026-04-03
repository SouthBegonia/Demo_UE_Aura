


#include "Game/LoadScreenSaveGame.h"

#include "AuraLogChannels.h"

FSavedMap ULoadScreenSaveGame::GetSavedMapWithMapName(const FString& InMapName)
{
	for (const FSavedMap& SavedMap : SavedMaps)
	{
		if (SavedMap.MapAssetName == InMapName)
			return SavedMap;
	}

	return FSavedMap();
}

bool ULoadScreenSaveGame::HasMap(const FString& InMapName) const
{
	return SavedMaps.ContainsByPredicate([&InMapName](const FSavedMap& SavedMap)
	{
		return SavedMap.MapAssetName == InMapName;
	});
}

void ULoadScreenSaveGame::PrintDebugInfo() const
{
	FString Description = FString("[ULoadScreenSaveGame] DEBUG_INFO:\n");

	Description.Append(FString::Printf(TEXT("PlayerName = %s, Level = %d\n"), *PlayerName, PlayerLevel));
	Description.Append(FString::Printf(TEXT("MapName = %s, MapAssetName = %s, PlayerStartTag = %s\n"), *MapName, *MapAssetName, *PlayerStartTag.ToString()));

	UE_LOG(LogAura_SaveGame, Log, TEXT("%s"), *Description);
}
