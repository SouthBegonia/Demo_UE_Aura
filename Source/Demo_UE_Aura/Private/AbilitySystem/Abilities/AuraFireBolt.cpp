


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

#pragma region Ability Description

FString UAuraFireBolt::GetAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageByDamageType(AbilityLevel, FAuraGameplayTags::Get().Damage_Fire);

	FString Description = FString::Printf(TEXT("<Title>FIRE BOLT</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	if (AbilityLevel == 1)
		Description.Append(FString::Printf(TEXT("<Default>Launches a bolt of fire, exploding on impact and dealing : </>")));
	else
		Description.Append(FString::Printf(TEXT("<Default>Launches %d bolt of fire, exploding on impact and dealing : </>"), FMath::Min(AbilityLevel, NumProjectiles)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> fire damage with a chance to burn</>\n\n")));

	return Description;
}

FString UAuraFireBolt::GetNextAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageByDamageType(AbilityLevel, FAuraGameplayTags::Get().Damage_Fire);

	FString Description = FString::Printf(TEXT("<Title>NEXT LEVEL :</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	Description.Append(FString::Printf(TEXT("<Default>Launches %d bolt of fire, exploding on impact and dealing : </>"), FMath::Min(AbilityLevel, NumProjectiles)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> fire damage with a chance to burn</>\n\n")));

	return Description;
}

#pragma endregion