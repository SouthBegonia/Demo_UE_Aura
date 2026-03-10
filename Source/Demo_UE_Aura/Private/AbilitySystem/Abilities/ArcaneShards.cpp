


#include "AbilitySystem/Abilities/ArcaneShards.h"

#pragma region Ability Description

FString UArcaneShards::GetAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>ARCANE SHARDS</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	if (AbilityLevel == 1)
		Description.Append(FString::Printf(TEXT("<Default>Summon shards of arcane energy, causing radial arcane damage of </>")));
	else
		Description.Append(FString::Printf(TEXT("<Default>Summon %d shards of arcane energy, causing radial arcane damage of </>"), FMath::Min(AbilityLevel, MaxNumShards)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> at the shard origins. </>\n\n")));

	return Description;
}

FString UArcaneShards::GetNextAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>NEXT LEVEL :</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	Description.Append(FString::Printf(TEXT("<Default>Summon %d shards of arcane energy, causing radial arcane damage of </>"), FMath::Min(AbilityLevel, MaxNumShards)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> at the shard origins. </>\n\n")));

	return Description;
}

#pragma endregion