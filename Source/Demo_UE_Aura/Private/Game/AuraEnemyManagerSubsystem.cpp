


#include "Game/AuraEnemyManagerSubsystem.h"

#include "AuraLogChannels.h"
#include "Actor/AuraEnemySpawnVolume.h"
#include "Character/AuraEnemy.h"
#include "Kismet/GameplayStatics.h"


void UAuraEnemyManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SpawnedEnemyCodeIndex = SpawnedEnemyCodeStartIndex;
	SpawnedEnemyCodeSet.Reset();
	bSystemEnabled = false;
}

void UAuraEnemyManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	SpawnedEnemyCodeIndex = SpawnedEnemyCodeStartIndex;
	SpawnedEnemyCodeSet.Reset();
	bSystemEnabled = false;
}

void UAuraEnemyManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAuraEnemySpawnVolume::StaticClass(), AllActors);
	if (AllActors.Num() > 0)
	{
		for (AActor* const& SpawnerActor : AllActors)
		{
			AAuraEnemySpawnVolume* SpawnVolume = Cast<AAuraEnemySpawnVolume>(SpawnerActor);
			// ...
		}
	}
	bSystemEnabled = AllActors.Num() > 0;
	UE_LOGFMT(LogAura_Enemy, Log, "[{FUNC}] : initialize system complete. bSystemEnabled = {enabled}", __FUNCTION__, bSystemEnabled);
	// TODO : handle for AAuraEnemySpawnVolume that creating in gaming
}

AAuraEnemy* UAuraEnemyManagerSubsystem::TrySpawnAuraEnemy(const UObject* WorldContextObject, const FAuraSpawnEnemyParameters& SpawnParams)
{
	//if (!bSystemEnabled) return nullptr;


	AAuraEnemy* Enemy = GetWorld()->SpawnActorDeferred<AAuraEnemy>(SpawnParams.EnemyClass, SpawnParams.SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);

	Enemy->SetEnemyLevel(SpawnParams.EnemyLevel);
	Enemy->SetCharacterClass(SpawnParams.EnemyCharacterClass);
	++SpawnedEnemyCodeIndex;
	Enemy->SetEnemyCode(SpawnedEnemyCodeIndex);

	Enemy->FinishSpawning(SpawnParams.SpawnTransform);
	Enemy->SpawnDefaultController();

	if (Enemy != nullptr)
	{
		SpawnedEnemyCodeSet.Add(SpawnedEnemyCodeIndex);
		UE_LOGFMT(LogAura_Enemy, Log, "[{FUNC}] : SpawnAuraEnemy success. EnemyCode = {code}", __FUNCTION__, SpawnedEnemyCodeIndex);
	}
	else
		UE_LOGFMT(LogAura_Enemy, Error, "[{FUNC}] : SpawnAuraEnemy failed. EnemyCode = {code}", __FUNCTION__, SpawnedEnemyCodeIndex);

	return Enemy;
}

void UAuraEnemyManagerSubsystem::HandleAuraEnemyDied(int32 EnemyCode)
{
	if (EnemyCode < SpawnedEnemyCodeStartIndex)
	{
		UE_LOGFMT(LogAura_Enemy, Warning, "[{FUNC}] : An unmanaged enemy died. EnemyCode = {code}", __FUNCTION__, EnemyCode);
		return;
	}

	if (SpawnedEnemyCodeSet.Contains(EnemyCode))
	{
		SpawnedEnemyCodeSet.Remove(EnemyCode);
		if (SpawnedEnemyCodeSet.Num() == 0)
			bSystemEnabled = false;

		UE_LOGFMT(LogAura_Enemy, Log, "[{FUNC}] : remove EnemyCode success. EnemyCode = {code}", __FUNCTION__, EnemyCode);
	}
	else
		UE_LOGFMT(LogAura_Enemy, Error, "[{FUNC}] : SpawnedEnemyCodeSet not contains EnemyCode = {code}", __FUNCTION__, EnemyCode);
}

void UAuraEnemyManagerSubsystem::ForEachAuraEnemyInWorld(const FForEachAuraEnemy& Delegate) const
{
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAuraEnemy::StaticClass(), AllActors);
	for (AActor* const& EnemyActor : AllActors)
	{
		AAuraEnemy* Enemy = Cast<AAuraEnemy>(EnemyActor);
		if (IsValid(Enemy))
		{
			if (!Delegate.ExecuteIfBound(Enemy))
				UE_LOG(LogAura_Enemy, Log, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}
