


#include "AbilitySystem/Data/LootTiers.h"

TArray<FlootItem> ULootTiers::GetLootItems()
{
	TArray<FlootItem> ReturnLootItems;

	for (const FlootItem& Item : LootItems)
	{
		for (int32 i = 0; i < Item.MaxNumberToSpawn; i++)
		{
			if (FMath::RandRange(1, 100) < Item.ChanceToSpawn)
			{
				FlootItem NewItem;
				NewItem.LootItemClass = Item.LootItemClass;
				NewItem.bLootLevelOverride = Item.bLootLevelOverride;

				ReturnLootItems.Add(NewItem);
			}
		}
	}

	return ReturnLootItems;
}
