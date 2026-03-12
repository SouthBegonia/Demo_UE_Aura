


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#pragma region Ability Description

FString UAuraFireBlast::GetAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>FIRE BLAST</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	Description.Append(FString::Printf(TEXT("<Default>Launches %d fire balls in all directions, each coming back and exploding upon return, causing </>"), FMath::Min(AbilityLevel, NumFireBalls)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> radial fire damage with a chance to burn</>\n\n")));

	return Description;
}

FString UAuraFireBlast::GetNextAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>NEXT LEVEL :</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	Description.Append(FString::Printf(TEXT("<Default>Launches %d fire balls in all directions, each coming back and exploding upon return, causing </>"), FMath::Min(AbilityLevel, NumFireBalls)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> radial fire damage with a chance to burn</>\n\n")));

	return Description;
}

#pragma endregion