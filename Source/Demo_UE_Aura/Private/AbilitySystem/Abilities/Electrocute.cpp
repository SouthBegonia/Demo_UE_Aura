


#include "AbilitySystem/Abilities/Electrocute.h"

#pragma region Ability Description

FString UElectrocute::GetAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>ELECTROCUTE</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	if (AbilityLevel == 1)
		Description.Append(FString::Printf(TEXT("<Default>Emits a beam of lighting, connecting with the target, repeatedly causing </>")));
	else
		Description.Append(FString::Printf(TEXT("<Default>Emits a beam of lighting, propagating to %d additional targets nearby, causing </>"), FMath::Min(AbilityLevel, MaxNumShockTarget)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> lightning damage with a chance to stun</>\n\n")));

	return Description;
}

FString UElectrocute::GetNextAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>NEXT LEVEL :</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	Description.Append(FString::Printf(TEXT("<Default>Emits a beam of lighting, propagating to %d additional targets nearby, causing </>"), FMath::Min(AbilityLevel, MaxNumShockTarget)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> lightning damage with a chance to stun</>\n\n")));

	return Description;
}

#pragma endregion