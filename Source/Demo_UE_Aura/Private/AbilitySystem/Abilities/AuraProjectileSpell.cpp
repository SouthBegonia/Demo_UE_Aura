


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;		// Fixed Pitch
		}
		// TODO : how to Adapt to Parabolic projectile?

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

		// Generate FGameplayEffectSpecHandle
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(Projectile);
		EffectContextHandle.SetAbility(this);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);
		// set all Types of Damage by CallerMagnitude
		for (const TPair<FGameplayTag, FScalableFloat>& Pair : DamageTypesMap)
		{
			const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaleDamage);
		}


		Projectile->DamageEffectSpecHandle = SpecHandle;

		// Generate Projectile Done
		Projectile->FinishSpawning(SpawnTransform);

		//UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("%s    ScaleDamage = %f"), *this->GetName(), ScaleDamage), true, true);
	}
}

#pragma region Ability Description

FString UAuraProjectileSpell::GetAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = DamageTypesMap.Find(FAuraGameplayTags::Get().Damage_Fire)->GetValueAtLevel(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>FIRE BOLT</>\n"));
	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n\n"), AbilityLevel));
	if (AbilityLevel == 1)
	{
		Description.Append(FString::Printf(TEXT("<Default>Launches a bolt of fire, exploding on impact and dealing : </>")));
	}
	else
	{
		Description.Append(FString::Printf(TEXT("<Default>Launches %d bolt of fire, exploding on impact and dealing : </>"), FMath::Min(AbilityLevel, NumProjectiles)));
	}

	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> fire damage with a chance to burn</>\n\n")));

	return Description;
}

FString UAuraProjectileSpell::GetNextAbilityDescription(int32 AbilityLevel)
{
	const int32 Damage = DamageTypesMap.Find(FAuraGameplayTags::Get().Damage_Fire)->GetValueAtLevel(AbilityLevel);

	FString Description = FString::Printf(TEXT("<Title>NEXT LEVEL :</>\n"));
	Description.Append(FString::Printf(TEXT("<Small>Level : </><Level>%d</>\n\n"), AbilityLevel));
	Description.Append(FString::Printf(TEXT("<Default>Launches %d bolt of fire, exploding on impact and dealing : </>"), FMath::Min(AbilityLevel, NumProjectiles)));
	Description.Append(FString::Printf(TEXT("<Damage>%d</>"), Damage));
	Description.Append(FString::Printf(TEXT("<Default> fire damage with a chance to burn</>\n\n")));

	return Description;
}

#pragma endregion