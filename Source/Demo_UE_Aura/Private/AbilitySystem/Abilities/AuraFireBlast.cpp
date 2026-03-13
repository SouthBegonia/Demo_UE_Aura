


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraFireBall.h"

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;

	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);


	for (const FRotator& Rotator : Rotators)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		FireBall->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBall->ExplosionDamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		FireBall->SetOwner(GetAvatarActorFromActorInfo());
		FireBall->FinishSpawning(SpawnTransform);
		FireBalls.Add(FireBall);
	}

	return FireBalls;
}

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