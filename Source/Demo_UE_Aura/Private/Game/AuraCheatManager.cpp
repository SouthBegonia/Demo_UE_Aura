


#include "Game/AuraCheatManager.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayCueNotifyTypes.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Character/AuraCharacter.h"
#include "Character/AuraEnemy.h"
#include "Game/AuraEnemyManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
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


/**
 * 添加指定EXP到Aura
 *	- 示例：AuraPlayer.AddExp 1000
 */
static FAutoConsoleCommandWithWorldAndArgs AuraAddExpToPlayer(
	TEXT("AuraPlayer.AddExp"),
	TEXT("Add the specified EXP to Aura"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray< FString >& Args, UWorld* World)
	{
		if (World)
		{
			if (Args.Num() == 0)
				return;

			const int32 Exp = FCString::Atoi(*Args[0]);
			if (Exp > 0)
			{
				auto ApplyAddExpGE = [](UAbilitySystemComponent* TargetASC, const int32 InEXP)
				{
					if (TargetASC == nullptr)
						return;

					// ===== Create GE =====
					FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
					EffectContext.AddSourceObject(TargetASC->GetOwner());

					UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("AddEXP")));
					Effect->DurationPolicy = EGameplayEffectDurationType::Instant;
					int32 Index = Effect->Modifiers.Num();
					Effect->Modifiers.Add(FGameplayModifierInfo());
					FGameplayModifierInfo& ModifierInfo = Effect->Modifiers[Index];
					ModifierInfo.ModifierMagnitude = FScalableFloat(InEXP);
					ModifierInfo.ModifierOp = EGameplayModOp::Additive;
					ModifierInfo.Attribute = UAuraAttributeSet::GetIncomingEXPAttribute();

					// ===== Apply GE =====
					if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContext, 1.f))
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
						UKismetSystemLibrary::PrintString(TargetASC, FString::Printf(TEXT("AuraPlayer.AddExp success. EXP = %d"), InEXP));
					}
				};

				TArray<AActor*> AllActors;
				UGameplayStatics::GetAllActorsOfClass(World, AAuraCharacter::StaticClass(), AllActors);
				for (AActor* const& PlayerActor : AllActors)
				{
					if (PlayerActor->ActorHasTag(FName("Player")))
					{
						UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(PlayerActor);
						ApplyAddExpGE(ASC, Exp);
					}
				}
			}
		}
	})
);
