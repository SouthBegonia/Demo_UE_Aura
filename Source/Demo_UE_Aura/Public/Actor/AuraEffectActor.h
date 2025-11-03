

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UAbilitySystemComponent;
class UGameplayEffect;
class USphereComponent;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy : uint8
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply,
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy : uint8
{
	RemoveOnEndOverlap,
	DoNotRemove,
};


UCLASS()
class DEMO_UE_AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:
	AAuraEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	bool bDestroyOnEffectRemoval = false;

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGamePlayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGamePlayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGamePlayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlap;


	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects")
	float ActorLevel = 1.f;
};
