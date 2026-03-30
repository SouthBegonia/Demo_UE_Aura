


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AuraLogChannels.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Game/AuraGameInstance.h"
#include "Game/AuraGameModeBase.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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

	// NiagaraComponent
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComp");
	//LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->AutoAttachParent = GetRootComponent();
	LevelUpNiagaraComponent->bAutoActivate = false;
}

// Server Only
void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, FString::Printf(TEXT("Server = %hhd"), HasAuthority()));

	// Init ability actor info for the Server
	InitAbilityActorInfo();
	InitHUD();

	LoadProgress();
}

// Client Only
void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, FString::Printf(TEXT("Client = %hhd"), HasAuthority()));

	// Init ability actor info for the Client
	InitAbilityActorInfo();
	InitHUD();
}

void AAuraCharacter::LoadProgress()
{
	bool bLoadSuccessful = false;

	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance()))
		{
			if (ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData())
			{
				auto InitializeInfoFunc = [this, &SaveData](const bool bDefaultInitialize)
				{
					AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState());
					UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(GetAbilitySystemComponent());

					// Init PlayerInfo
					AuraPlayerState->SetPlayerLevel(bDefaultInitialize ? 1 : SaveData->PlayerLevel);
					AuraPlayerState->SetPlayerEXP(bDefaultInitialize ? 0 : SaveData->PlayerEXP);
					AuraPlayerState->SetAttributePoints(bDefaultInitialize ? 0 : SaveData->AttributePoints);
					AuraPlayerState->SetSpellPoints(bDefaultInitialize? 0 : SaveData->SpellPoints);

					// Init AttributeValue
					if (bDefaultInitialize)
						InitializeDefaultAttributes();
					else
						InitializeSavedGameAttributes(*SaveData);


					// Add CharacterAbility
					if (bDefaultInitialize)
						// add CharacterAbility and setting Level with DefaultConfig
						AddCharacterAbilities();
					else
					{
						// add CharacterAbility and setting Level with SaveGame data
						AuraASC->AddCharacterAbilitiesFromSaveData(SaveData);
					}

					if (bDefaultInitialize)
						UE_LOGFMT(LogAura, Log, "[{FUNC}] : Initialize Attributes by DefaultAttributes for first time login.", __FUNCTION__);
					else
						UE_LOGFMT(LogAura, Log, "[{FUNC}] : Initialize Attributes by SaveData for each time login.", __FUNCTION__);
				};

				// This means these AttributeValue had never been initialized, we have to initialize them with DefaultConfig
				if (SaveData->bIsFirstTimeLoadIn)

					InitializeInfoFunc(true);
				else
				{
					// This means these AttributeValue had been initialized before, they are valid value in SaveGame, we need initialize them with value from SaveGame
					if (!AuraGameInstance->IsInitializedPlayerInfoFromGameSave())
						InitializeInfoFunc(false);
				}

				AuraGameInstance->MarkInitializedPlayerInfoFromGameSave();
				bLoadSuccessful = true;
			}
		}
	}

	if (!bLoadSuccessful)
		UE_LOGFMT(LogAura, Log, "[{FUNC}] : Load progress failed.", __FUNCTION__);
}

int32 AAuraCharacter::GetPlayerLevel_Implementation()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsStunned)
	{
		BurnDebuffNiagaraComponent->Activate();
	}
	else
	{
		BurnDebuffNiagaraComponent->Deactivate();
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_CursorTrace);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputPressed);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputHeld);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputReleased);

		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffNiagaraComponent->Activate();
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffNiagaraComponent->Deactivate();
		}
	}
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

	// Update Ability Status after LevelUp
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatus(AuraPlayerState->GetPlayerLevel());
	}
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	AuraPlayerState->AddAttributePoints(InPoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	AuraPlayerState->AddSpellPoints(InPoints);
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

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial) const
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	if (AuraPlayerController)
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial);

		AuraPlayerController->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation() const
{
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	if (AuraPlayerController)
	{
		AuraPlayerController->HideMagicCircle();

		AuraPlayerController->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	bool SaveSuccessful = false;

	if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
	{
		UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
		if (ULoadScreenSaveGame* SaveData = AuraGameMode->RetrieveInGameSaveData())
		{
			// Modify SaveData
			FSaveGameModifiableParams ModifyParams;
			ModifyParams.PlayerStartTag = CheckpointTag;
			AuraGameMode->ModifyInGameSaveData(*SaveData, ModifyParams, GetPlayerState());

			// Save SaveData
			const FString SlotName = AuraGameInstance->LoadSlotName;
			const int32 SlotIndex = AuraGameInstance->LoadSlotIndex;
			SaveSuccessful = AuraGameMode->SaveInGameProgressData(*SaveData, SlotName, SlotIndex);

			// Update InGame Data
			//	- Not necessary to set PlayerStartTag, because it will be set when next UMVVM_VM_LoadScreen::SlotButtonPressed_Play (data will be loaded from local).
			//		anyway, just set it for other possible uses (ex: Recording last PlayerStartTag)
			AuraGameInstance->PlayerStartTag = CheckpointTag;
		}
	}
}

// Execute On Server
void AAuraCharacter::LevelUp_Implementation()
{
	// Show LevelUp Effect
	MulticastLevelUpParticles();
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = CameraComp->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();

		const FRotator ToCameraRotation = UKismetMathLibrary::FindLookAtRotation(NiagaraSystemLocation, CameraLocation);
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);

		LevelUpNiagaraComponent->Activate(true);
	}
}

int32 AAuraCharacter::FindLevelForEXp_Implementation(int32 InEXP) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return AuraPlayerState->LevelUpInfo->FindLevelForEXP(InEXP);
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return  AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)

	return  AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState)
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	OnAscRegisteredDelegate.Broadcast(AbilitySystemComponent);
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Type_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
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
