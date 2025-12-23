


#include "AbilitySystem/Data/LevelUpInfo.h"

#include "AuraLogChannels.h"

ULevelUpInfo::ULevelUpInfo()
{
	if (LevelUpInformation.Num() == 0)
		LevelUpInformation.Add(FAuraLevelUpInfo());	// LevelUpInformation[0] is just a placeholder

	// TODO check the validity of the original data ?
}

int32 ULevelUpInfo::GetLevelUpRequirement(const int32 Level) const
{
	if (Level == 0)
		return 0;

	check(LevelUpInformation.IsValidIndex(Level))

	return LevelUpInformation[Level].LevelUpRequirement;
}

int32 ULevelUpInfo::FindLevelForEXP(const int32 EXP) const
{
	check(EXP >= 0);

	const int32 MaxLevel = LevelUpInformation.Num() - 1;
	for (int32 Level = 1; Level < MaxLevel; Level++)
	{
		if (EXP < LevelUpInformation[Level].LevelUpRequirement)
			return Level;

		if (Level == MaxLevel)
			return MaxLevel;
	}

	UE_LOGFMT(LogAura, Log, "[{FUNC}] : can't find the Level for target EXP={EXP}", __FUNCTION__, EXP);
	return 1;
}
