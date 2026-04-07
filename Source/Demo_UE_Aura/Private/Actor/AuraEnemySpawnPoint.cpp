


#include "Actor/AuraEnemySpawnPoint.h"

#include "Character/AuraEnemy.h"
#include "Game/AuraEnemyManagerSubsystem.h"

AAuraEnemy* AAuraEnemySpawnPoint::SpawnEnemy()
{
	AAuraEnemy* Enemy = nullptr;

	if (UAuraEnemyManagerSubsystem* EnemyManagerSubsystem = GetWorld()->GetSubsystem<UAuraEnemyManagerSubsystem>())
	{
		FAuraSpawnEnemyParameters AuraSpawnEnemyParameters(EnemyClass, EnemyLevel, CharacterClass);
		AuraSpawnEnemyParameters.SpawnTransform = GetActorTransform();

		Enemy = EnemyManagerSubsystem->TrySpawnAuraEnemy(this, AuraSpawnEnemyParameters);
	}

	return Enemy;
}
