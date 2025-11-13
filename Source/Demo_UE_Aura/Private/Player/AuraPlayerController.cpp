


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

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

	CursorHitOnTick();

	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::CursorHitOnTick()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHitResultOnTick);
}

void AAuraPlayerController::CursorTrace()
{
	if (!CursorHitResultOnTick.bBlockingHit)
		return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHitResultOnTick.GetActor());

	//if (CursorHitResultOnTick.GetActor())
	//	UE_LOG(LogTemp, Log, TEXT("CursorHitResultOnTick.GetActor = %s"), *CursorHitResultOnTick.GetActor()->GetName());

	if (LastActor != ThisActor)
	{
		if (LastActor)
			LastActor->UnHighlightActor();
		if (ThisActor)
			ThisActor->HighlightActor();
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

	// TODO : Stop AutoRun?
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString::Printf(TEXT("Pressed    InputTag = %s"), *InputTag.ToString()));

	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Red, FString::Printf(TEXT("Released    InputTag = %s"), *InputTag.ToString()));
	if (GetASC() == nullptr)
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
	if (!bTargeting && !bShiftKeyDown)
	{
		APawn* ControlledPawn = GetPawn<APawn>();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// Click LMB to Move : Navigation
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
				}else
				{
					bAutoRunning = false;
				}
			}
		}

		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Black, FString::Printf(TEXT("Held    InputTag = %s"), *InputTag.ToString()));

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
	if (bTargeting || bShiftKeyDown)
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
