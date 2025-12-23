


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraLogChannels.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetCapsuleComponent());

	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComp->SetupAttachment(SpringArm);

	CharacterClass = ECharacterClass::Elementalist;
}

// Server Only
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, FString::Printf(TEXT("Server = %hhd"), HasAuthority()));
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

// Client Only
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, FString::Printf(TEXT("Client = %hhd"), HasAuthority()));

	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	return AuraPlayerState->GetPlayerLevel();
}

int32 AAuraCharacter::GetEXP_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return AuraPlayerState->GetPlayerEXP();
}

void AAuraCharacter::AddToEXP_Implementation(int32 InEXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	AuraPlayerState->AddToPlayerEXP(InEXP);
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	AuraPlayerState->AddToLevel(InLevel);
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InPoints)
{
	// TODO : Add AttributePoints to PlayerState
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InPoints)
{
	// TODO : Add SpellPoints to PlayerState
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 InLevel) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	if (AuraPlayerState->LevelUpInfo->LevelUpInformation.IsValidIndex(InLevel))
		return AuraPlayerState->LevelUpInfo->LevelUpInformation[InLevel].AttributePointAward;

	UE_LOGFMT(LogAura, Error, "[{FUNC}] : can't find the LevelUpInformation[{InLevel}].AttributePointAward", __FUNCTION__,  InLevel);
	return 0;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 InLevel) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	if (AuraPlayerState->LevelUpInfo->LevelUpInformation.IsValidIndex(InLevel))
		return AuraPlayerState->LevelUpInfo->LevelUpInformation[InLevel].SpellPointAward;

	UE_LOGFMT(LogAura, Error, "[{FUNC}] : can't find the LevelUpInformation[{InLevel}].SpellPointAward", __FUNCTION__,  InLevel);
	return 0;
}

void AAuraCharacter::LevelUp_Implementation()
{
	//TODO
}

int32 AAuraCharacter::FindLevelForEXp_Implementation(int32 InEXP) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return AuraPlayerState->LevelUpInfo->FindLevelForEXP(InEXP);
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	InitHUD();

	// initial Attributes by GE
	InitializeDefaultAttributes();
}

void AAuraCharacter::InitHUD()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	if (AuraPlayerController)	// check for multiplayer
	{
		AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD());
		if (AuraHUD)
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}
