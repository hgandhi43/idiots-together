#include "IdiotsTogetherPlayerController.h"
#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherCharacterRules.h"
#include "IdiotsTogetherGameModeBase.h"
#include "IdiotsTogetherSettings.h"
#include "IdiotsTogether/UI/IdiotsTogetherInteractionWidget.h"
#include "IdiotsTogether/UI/IdiotsTogetherDevCharSelectWidget.h"
#include "IdiotsTogether/UI/IdiotsTogetherPhaseHUDWidget.h"
#include "IdiotsTogether/IdiotsTogether.h"
#include "UObject/ConstructorHelpers.h"

AIdiotsTogetherPlayerController::AIdiotsTogetherPlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Default;

	static ConstructorHelpers::FClassFinder<UIdiotsTogetherInteractionWidget> WidgetClassFinder(TEXT("/Game/UI/WBP_IdiotsTogetherInteractionPrompt.WBP_IdiotsTogetherInteractionPrompt_C"));
	if (WidgetClassFinder.Succeeded())
	{
		InteractionWidgetClass = WidgetClassFinder.Class;
	}
}

void AIdiotsTogetherPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	if (InteractionWidgetClass)
	{
		InteractionWidgetInstance = CreateWidget<UIdiotsTogetherInteractionWidget>(this, InteractionWidgetClass);
		if (InteractionWidgetInstance)
		{
			InteractionWidgetInstance->AddToViewport(InteractionPromptZOrder);
			InteractionWidgetInstance->HidePrompt();
		}
	}

#if !UE_BUILD_SHIPPING
	{
		TSubclassOf<UIdiotsTogetherDevCharSelectWidget> WidgetClassToUse = DevCharSelectWidgetClass;
		if (!WidgetClassToUse)
		{
			WidgetClassToUse = UIdiotsTogetherDevCharSelectWidget::StaticClass();
		}

		DevCharSelectWidgetInstance = CreateWidget<UIdiotsTogetherDevCharSelectWidget>(this, WidgetClassToUse);
		if (DevCharSelectWidgetInstance)
		{
			DevCharSelectWidgetInstance->AddToViewport(DevWidgetZOrder);
		}
		else
		{
			UE_LOG(LogIdiotsTogetherUI, Error, TEXT("Failed to create dev character-select widget on %s"), *GetName());
		}
	}
#endif

	TSubclassOf<UIdiotsTogetherPhaseHUDWidget> PhaseWidgetClassToUse = PhaseHUDWidgetClass;
	if (!PhaseWidgetClassToUse)
	{
		PhaseWidgetClassToUse = UIdiotsTogetherPhaseHUDWidget::StaticClass();
	}

	PhaseHUDWidgetInstance = CreateWidget<UIdiotsTogetherPhaseHUDWidget>(this, PhaseWidgetClassToUse);
	if (PhaseHUDWidgetInstance)
	{
		PhaseHUDWidgetInstance->AddToViewport(PhaseHUDZOrder);
	}
	else
	{
		UE_LOG(LogIdiotsTogetherUI, Error, TEXT("Failed to create phase HUD widget on %s"), *GetName());
	}
}

void AIdiotsTogetherPlayerController::ShowInteractionPrompt(const FText& Text)
{
	if (InteractionWidgetInstance)
	{
		InteractionWidgetInstance->SetPromptText(Text);
		InteractionWidgetInstance->ShowPrompt();
	}
}

void AIdiotsTogetherPlayerController::HideInteractionPrompt()
{
	if (InteractionWidgetInstance)
	{
		InteractionWidgetInstance->HidePrompt();
	}
}

void AIdiotsTogetherPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

#if !UE_BUILD_SHIPPING
	if (InputComponent)
	{
		// Character selection keys live here until the lobby (P1.3) provides real UI.
		InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectBoss);
		InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectNerd);
		InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectMuscle);
		InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectIntern);
		InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectRandom);
		InputComponent->BindKey(EKeys::Six, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevAdvancePhase);
		InputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevForceSuccess);
		InputComponent->BindKey(EKeys::Eight, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevForceFailure);
		InputComponent->BindKey(EKeys::Nine, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevCompleteNextObjective);
		InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &AIdiotsTogetherPlayerController::ToggleDevCursor);
	}
#endif
}

void AIdiotsTogetherPlayerController::ToggleDevCursor()
{
	bShowMouseCursor = !bShowMouseCursor;
	if (bShowMouseCursor)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}

// ---------------------------------------------------------------------------
// Character selection
// ---------------------------------------------------------------------------

void AIdiotsTogetherPlayerController::ServerRequestCharacterSelection_Implementation(EIdiotsTogetherCharacterType Selection)
{
	UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("Character selection request %d from %s"),
		static_cast<int32>(Selection), *GetName());

	if (AIdiotsTogetherPlayerState* PS = GetPlayerState<AIdiotsTogetherPlayerState>())
	{
		PS->SelectCharacter_Server(Selection);
	}
	else
	{
		UE_LOG(LogIdiotsTogetherPlayer, Warning, TEXT("Character selection from %s ignored: no PlayerState"), *GetName());
	}
}

bool AIdiotsTogetherPlayerController::ServerRequestCharacterSelection_Validate(EIdiotsTogetherCharacterType Selection)
{
	// Out-of-range enum values can only come from a modified client.
	return IdiotsTogetherCharacterRules::IsValidSelectionRequest(Selection);
}

void AIdiotsTogetherPlayerController::DevSelectBoss()   { ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Boss); }
void AIdiotsTogetherPlayerController::DevSelectNerd()   { ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Nerd); }
void AIdiotsTogetherPlayerController::DevSelectMuscle() { ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Muscle); }
void AIdiotsTogetherPlayerController::DevSelectIntern() { ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Intern); }
void AIdiotsTogetherPlayerController::DevSelectRandom() { ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Random); }

// ---------------------------------------------------------------------------
// Dev commands (never active in Shipping)
// ---------------------------------------------------------------------------

void AIdiotsTogetherPlayerController::DevAdvancePhase()          { ServerDevCommand(EIdiotsTogetherDevCommand::AdvancePhase); }
void AIdiotsTogetherPlayerController::DevForceSuccess()          { ServerDevCommand(EIdiotsTogetherDevCommand::ForceSuccess); }
void AIdiotsTogetherPlayerController::DevForceFailure()          { ServerDevCommand(EIdiotsTogetherDevCommand::ForceFailure); }
void AIdiotsTogetherPlayerController::DevCompleteNextObjective() { ServerDevCommand(EIdiotsTogetherDevCommand::CompleteNextObjective); }

bool AIdiotsTogetherPlayerController::IsDevCommandAllowed() const
{
#if UE_BUILD_SHIPPING
	return false;
#else
	// On a listen server the host's controller is the only local one on the server.
	return IsLocalController() || UIdiotsTogetherSettings::Get()->bAllowClientDevCommands;
#endif
}

bool AIdiotsTogetherPlayerController::ServerDevCommand_Validate(EIdiotsTogetherDevCommand Command)
{
#if UE_BUILD_SHIPPING
	return false; // Dev commands do not exist in Shipping; a client sending one is cheating.
#else
	return true;
#endif
}

void AIdiotsTogetherPlayerController::ServerDevCommand_Implementation(EIdiotsTogetherDevCommand Command)
{
#if !UE_BUILD_SHIPPING
	if (!IsDevCommandAllowed())
	{
		UE_LOG(LogIdiotsTogetherDev, Warning, TEXT("Rejected dev command %d from non-host %s"), static_cast<int32>(Command), *GetName());
		return;
	}

	AIdiotsTogetherGameModeBase* GM = GetWorld() ? GetWorld()->GetAuthGameMode<AIdiotsTogetherGameModeBase>() : nullptr;
	if (!GM)
	{
		return;
	}

	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("Dev command %s from %s"), *UEnum::GetValueAsString(Command), *GetName());

	switch (Command)
	{
	case EIdiotsTogetherDevCommand::AdvancePhase:          GM->AdvancePhase(); break;
	case EIdiotsTogetherDevCommand::ForceSuccess:          GM->ForcePhaseSuccess(); break;
	case EIdiotsTogetherDevCommand::ForceFailure:          GM->ForcePhaseFailure(); break;
	case EIdiotsTogetherDevCommand::CompleteNextObjective: GM->DevCompleteNextObjective(); break;
	default: break;
	}
#endif
}
