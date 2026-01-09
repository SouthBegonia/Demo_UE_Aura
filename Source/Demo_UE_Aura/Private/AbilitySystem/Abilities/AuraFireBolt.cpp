


#include "AbilitySystem/Abilities/AuraFireBolt.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTargetActor)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	check(GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())

	int32 SpawnProjectilesCount = GetNumProjectiles();
	if (DebugNumProjectiles > 0)
		SpawnProjectilesCount = DebugNumProjectiles;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	if (SpawnProjectilesCount == 1)
	{
		FAuraSpawnProjectileParams SpawnParams;
		SpawnParams.SpawnLocation = SocketLocation;
		SpawnParams.SpawnRotation = Rotation;
		SpawnParams.HomingTargetActor = HomingTargetActor;
		SpawnParams.HomingTargetLocation = ProjectileTargetLocation;
		SpawnParams.HomingAcceleration = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);

		GenerateAndSpawnProjectile(SpawnParams);
	}
	else
	{
		const FVector Forward = Rotation.Vector();
		TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, SpawnProjectilesCount);

		for (FRotator& Rot : Rotations)
		{
			FAuraSpawnProjectileParams SpawnParams;
			SpawnParams.SpawnLocation = SocketLocation;
			SpawnParams.SpawnRotation = Rot;
			SpawnParams.HomingTargetActor = HomingTargetActor;
			SpawnParams.HomingTargetLocation = ProjectileTargetLocation;
			SpawnParams.HomingAcceleration = FMath::RandRange(HomingAccelerationMin, HomingAccelerationMax);

			GenerateAndSpawnProjectile(SpawnParams);
		}

		if (bDrawDebugLine)
		{
			// Boundary
			const FVector StartLocation = SocketLocation + FVector::UpVector * -30.f;
			const FVector LeftOfSpread = Forward.RotateAngleAxis(-ProjectileSpread / 2.f, FVector::UpVector);
			const FVector RightOfSpread = Forward.RotateAngleAxis(ProjectileSpread / 2.f, FVector::UpVector);
			UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), StartLocation, StartLocation + Rotation.Vector() * 100.f, 0, FLinearColor::White, 20, 1);
			UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), StartLocation, StartLocation + LeftOfSpread * 100.f, 0, FLinearColor::White, 20, 1);
			UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), StartLocation, StartLocation + RightOfSpread * 100.f, 0, FLinearColor::White, 20, 1);
		}
	}
}

#pragma region Ability Description

FString UAuraFireBolt::GetAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>FIRE BOLT</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	if (AbilityLevel == 1)
		Description.Append(FString::Printf(TEXT("<Default>Launches a bolt of fire, exploding on impact and dealing : </>")));
	else
		Description.Append(FString::Printf(TEXT("<Default>Launches %d bolt of fire, exploding on impact and dealing : </>"), FMath::Min(AbilityLevel, MaxNumProjectiles)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> fire damage with a chance to burn</>\n\n")));

	return Description;
}

FString UAuraFireBolt::GetNextAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageValue(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>NEXT LEVEL :</>\n"));

	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Small>ManaCost : </><ManaCost>%.1f</>\n"), GetManaCost(AbilityLevel)));
	Description.Append(FString::Printf(TEXT("<Small>Cooldown : </><Cooldown>%.1fs</>\n"), GetCooldown(AbilityLevel)));

	Description.Append(TEXT("\n"));
	Description.Append(FString::Printf(TEXT("<Default>Launches %d bolt of fire, exploding on impact and dealing : </>"), FMath::Min(AbilityLevel, MaxNumProjectiles)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> fire damage with a chance to burn</>\n\n")));

	return Description;
}

#pragma endregion