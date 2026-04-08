


#include "Game/AuraCheatManager.h"

#include "Character/AuraEnemy.h"
#include "Game/AuraEnemyManagerSubsystem.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraCheatManager::AuraEnemyFreeze() const
{
	if (UAuraEnemyManagerSubsystem* EnemyManager = GetWorld()->GetSubsystem<UAuraEnemyManagerSubsystem>())
	{
		FForEachAuraEnemy Delegate;
		Delegate.BindLambda([this](AAuraEnemy* const AuraEnemy)
		{
			AuraEnemy->EnemyDebugState = EEnemyDebugState::Idle;
		});
		EnemyManager->ForEachAuraEnemyInWorld(Delegate);
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AuraEnemyFreeze Done.")));
	}
}

void UAuraCheatManager::AuraEnemyFreezeRecover()
{
	if (UAuraEnemyManagerSubsystem* EnemyManager = GetWorld()->GetSubsystem<UAuraEnemyManagerSubsystem>())
	{
		FForEachAuraEnemy Delegate;
		Delegate.BindLambda([this](AAuraEnemy* const AuraEnemy)
		{
			AuraEnemy->EnemyDebugState = EEnemyDebugState::None;
		});
		EnemyManager->ForEachAuraEnemyInWorld(Delegate);
		UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("AuraEnemyFreezeRecover Done.")));
	}
}
