

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

	virtual void Tick(float DeltaSeconds) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GamePlayEffectClass);

	UFUNCTION(BlueprintCallable)
	void SetActorLevel(const float InActorLevel) { ActorLevel = InActorLevel; };

	/*
	 * Decide should this Actor be destroyed after Applied GE
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects", meta=(ToolTip="Decide should this Actor be destroyed after Applied GE"))
	bool bDestroyOnEffectRemoval = false;

	/*
	 * Decide whether GE can be Applied to EnemyActor(witch has tag "Enemy")
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Aura|Applied Effects", meta=(ToolTip="Decide whether GE can be Applied to EnemyActor(witch has tag \"Enemy\")"))
	bool bApplyEffectToEnemies = false;

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

#pragma region Pickup Effect

public:
	UFUNCTION(BlueprintCallable)
	void StartRotation();

	UFUNCTION(BlueprintCallable)
	void StartSinusoidalMovement();


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Pickup Movement")
	bool bRotates = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Pickup Movement", meta=(EditCondition="bRotates"))
	float RotationRate = 45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Pickup Movement")
	bool bSinusoidalMovement = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Pickup Movement", meta=(EditCondition="bSinusoidalMovement"))
	float SineAmplitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Aura|Pickup Movement", meta=(EditCondition="bSinusoidalMovement"))
	float SinePeriodConstant = 1.f;


	UPROPERTY(BlueprintReadWrite, Category="Aura|Pickup Movement")
	FVector CalculatedLocation;
	UPROPERTY(BlueprintReadWrite, Category="Aura|Pickup Movement")
	FRotator CalculatedRotation;

	UPROPERTY(BlueprintReadOnly, Category="Aura|Pickup Movement")
	FVector InitialLocation;

private:
	float RunningTime = 0.f;

	void ItemMovement(float DeltaTime);

#pragma endregion
};
