


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
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

	check(GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	FAuraSpawnProjectileParams SpawnParams;
	SpawnParams.SpawnLocation = ProjectileTargetLocation;
	SpawnParams.SpawnRotation = Rotation;
	GenerateAndSpawnProjectile(SpawnParams);

	if (bDrawDebugLine)
	{
		// Projectile Direction
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), SocketLocation, SocketLocation + Rotation.Vector() * 100.f, 2, FLinearColor::Green, 20, 2);
	}
}

void UAuraProjectileSpell::GenerateAndSpawnProjectile(const FAuraSpawnProjectileParams& SpawnParams) const
{
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnParams.SpawnLocation);
	SpawnTransform.SetRotation(SpawnParams.SpawnRotation.Quaternion());

	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	const AActor* LocalHomingActor = SpawnParams.HomingTargetActor.IsValid() && SpawnParams.HomingTargetActor.Get()->Implements<UCombatInterface>() ? SpawnParams.HomingTargetActor.Get() : nullptr;
	const FVector LocalHomingLocation = SpawnParams.HomingTargetLocation;
	const bool bHoming = IsValid(LocalHomingActor) || !LocalHomingLocation.IsZero();
	if (bHoming)
	{
		Projectile->ProjectileMovement->HomingAccelerationMagnitude = SpawnParams.HomingAcceleration;

		if (IsValid(LocalHomingActor))
		{
			// Homing a specific object
			Projectile->ProjectileMovement->HomingTargetComponent = LocalHomingActor->GetRootComponent();
		}
		else
		{
			// Homing a new SceneComponent with specific location
			USceneComponent* HomingLocationComp = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent = HomingLocationComp;
			Projectile->HomingTargetSceneComponent->SetWorldLocation(LocalHomingLocation);

			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}

		if (bDrawDebugLine)
		{
			UKismetSystemLibrary::DrawDebugSphere(GetAvatarActorFromActorInfo(), Projectile->ProjectileMovement->HomingTargetComponent.Get()->GetComponentLocation(), 100, 12, FLinearColor::Green, 5.f, 1.f);
		}
	}
	Projectile->ProjectileMovement->bIsHomingProjectile = bHoming;


	// Generate FDamageEffectParams, then will Apply GE Damage on AAuraProjectile::OnSphereOverlap()
	FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);  // Only after AAuraProjectile::OnSphereOverlap() will get TargetActor
	Projectile->DamageEffectParams = DamageEffectParams;


	// Generate Projectile Done
	Projectile->FinishSpawning(SpawnTransform);
}
