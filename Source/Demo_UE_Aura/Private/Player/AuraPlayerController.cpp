


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Components/SplineComponent.h"
#include "Demo_UE_Aura/Demo_UE_Aura.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraInputContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)	// check for multiplayer
	{
		Subsystem->AddMappingContext(AuraInputContext, 0);
	}
	//check(Subsystem); // check for singleplayer


	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;


	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}


void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	UpdateMagicCircleLocation();
	AutoRun();
}

void AAuraPlayerController::CursorHitOnTick()
{
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_EXCLUDE_PLAYER : ECC_Visibility;

	GetHitResultUnderCursor(TraceChannel, false, CursorHitResultOnTick);
}

void AAuraPlayerController::CursorTrace()
{
	// CursorTrace Block
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		HighlightTargetActor(false, LastHighlightActor.Get());
		HighlightTargetActor(false, CurrentHighlightActor.Get());

		LastHighlight = nullptr;
		CurrentHighlight = nullptr;

		LastHighlightActor = nullptr;
		CurrentHighlightActor = nullptr;

		return;
	}

	CursorHitOnTick();
	if (!CursorHitResultOnTick.bBlockingHit)
		return;

	AActor* CursorHitActor = CursorHitResultOnTick.GetActor();

	LastHighlightActor = CurrentHighlightActor;
	CurrentHighlightActor = (CursorHitActor != nullptr && CursorHitActor->Implements<UHighlightInterface>()) ? CursorHitActor : nullptr;

	LastHighlight = CurrentHighlight;
	CurrentHighlight = CurrentHighlightActor.IsValid() ? Cast<IHighlightInterface>(CurrentHighlightActor.Get()) : nullptr;

	//if (CursorHitResultOnTick.GetActor())
	//	UE_LOG(LogTemp, Log, TEXT("CursorHitResultOnTick.GetActor = %s"), *CursorHitResultOnTick.GetActor()->GetName());

	if (LastHighlightActor != CurrentHighlightActor)
	{
		HighlightTargetActor(false, LastHighlightActor.Get());
		HighlightTargetActor(true, CurrentHighlightActor.Get());
	}
}

void AAuraPlayerController::HighlightTargetActor(bool bHighlight, AActor* TargetActor)
{
	if (!IsValid(TargetActor))
		return;

	if (TargetActor->Implements<UHighlightInterface>())
	{
		if (bHighlight)
			IHighlightInterface::Execute_HighlightActor(TargetActor);
		else
			IHighlightInterface::Execute_UnHighlightActor(TargetActor);
	}
}


void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)
		return;

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}


void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
		return;

	const FVector2D InputAxisVector2D = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector2D.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector2D.X);
	}

	// stop AutoRun when get input from Controller
	bAutoRunning = false;
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString::Printf(TEXT("Pressed    InputTag = %s"), *InputTag.ToString()));

	if (GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
		return;

	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (CurrentHighlight != nullptr)
		{
			if (CurrentHighlightActor.Get()->Implements<UEnemyInterface>())
				TargetingStatus = ETargetingStatus::TargetingEnemy;
			else
				TargetingStatus = ETargetingStatus::TargetingMapEntrance;
		}else
			TargetingStatus = ETargetingStatus::NotTargeting;
		bAutoRunning = false;
	}

	if (GetASC())
		GetASC()->AbilityInputTagPressed(InputTag);
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, FString::Printf(TEXT("Released    InputTag = %s"), *InputTag.ToString()));
	if (GetASC() == nullptr)
		return;

	if (GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
		return;

	GetASC()->AbilityInputTagReleased(InputTag);

	// released
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	if (GetASC())
	{
		// released LMB to activate GA to Target
		GetASC()->AbilityInputTagReleased(InputTag);
	}

	// released LMB
	if (TargetingStatus != ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		APawn* ControlledPawn = GetPawn<APawn>();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// Click LMB to Move : Navigation
			if (GetASC() && !GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
			{
				bool bShowClickNiagaraSys = true;

				// Modify CachedDestination
				if (CurrentHighlight != nullptr && CurrentHighlightActor.IsValid())
				{
					IHighlightInterface::Execute_SetMoveToLocation(CurrentHighlightActor.Get(), CachedDestination);
					bShowClickNiagaraSys = false;
				}

				if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
				{
					Spline->ClearSplinePoints();
					for (const FVector& PathPoint : NavPath->PathPoints)
					{
						Spline->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World);
						//DrawDebugSphere(GetWorld(), PathPoint, 8.f, 8, FColor::Green, false, 5.f);
					}
					if (Spline->GetNumberOfSplinePoints() > 0)
					{
						CachedDestination = NavPath->PathPoints.Last();
						bAutoRunning = true;

						if (bShowClickNiagaraSys)
							UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
					}else
					{
						bAutoRunning = false;
					}
				}
			}
		}

		FollowTime = 0.f;
		TargetingStatus = ETargetingStatus::NotTargeting;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Black, FString::Printf(TEXT("Held    InputTag = %s"), *InputTag.ToString()));

	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
		return;

	// held
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	// held LMB
	if (TargetingStatus == ETargetingStatus::TargetingEnemy || bShiftKeyDown)
	{
		if (GetASC())
		{
			// held LMB to activate GA to Target
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		// held LMB to Move : AddMovementInput
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHitResultOnTick.bBlockingHit)
		{
			CachedDestination = CursorHitResultOnTick.ImpactPoint;
		}
		if (APawn* ControlledPawn = GetPawn<APawn>())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bIsBlockedHit, bool bIsCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageTextComp = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComp->RegisterComponent();

		DamageTextComp->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		DamageTextComp->SetDamageText(DamageAmount, bIsBlockedHit, bIsCriticalHit);
	}
}

void AAuraPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if (!IsValid(MagicCircle))
	{
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);

		if (DecalMaterial != nullptr)
			MagicCircle->MagicCircleDecalComp->SetMaterial(0, DecalMaterial);
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->SetActorLocation(CursorHitResultOnTick.ImpactPoint);
	}
}
