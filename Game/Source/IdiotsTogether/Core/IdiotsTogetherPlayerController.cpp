#include "IdiotsTogetherPlayerController.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "IdiotsTogetherPlayerState.h"
#include "IdiotsTogetherGameModeBase.h"
#include "IdiotsTogether/UI/IdiotsTogetherInteractionWidget.h"
#include "IdiotsTogether/UI/IdiotsTogetherDevCharSelectWidget.h"
#include "IdiotsTogether/UI/IdiotsTogetherPhaseHUDWidget.h"
#include "IdiotsTogether/IdiotsTogether.h"


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

	if (IsLocalController())
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
	}

	if (IsLocalController() && InteractionWidgetClass)
	{
		InteractionWidgetInstance = CreateWidget<UIdiotsTogetherInteractionWidget>(this, InteractionWidgetClass);
		if (InteractionWidgetInstance)
		{
			InteractionWidgetInstance->AddToViewport(10);
			InteractionWidgetInstance->HidePrompt();
		}
	}

	if (IsLocalController())
	{
		TSubclassOf<UIdiotsTogetherDevCharSelectWidget> WidgetClassToUse = DevCharSelectWidgetClass;
		if (!WidgetClassToUse)
		{
			WidgetClassToUse = UIdiotsTogetherDevCharSelectWidget::StaticClass();
		}

		DevCharSelectWidgetInstance = CreateWidget<UIdiotsTogetherDevCharSelectWidget>(this, WidgetClassToUse);
		if (DevCharSelectWidgetInstance)
		{
			DevCharSelectWidgetInstance->AddToViewport(5);
			UE_LOG(LogIdiotsTogetherUI, Verbose, TEXT("Dev character-select widget created on %s"), *GetName());
		}
		else
		{
			UE_LOG(LogIdiotsTogetherUI, Error, TEXT("Failed to create dev character-select widget on %s"), *GetName());
		}

		TSubclassOf<UIdiotsTogetherPhaseHUDWidget> PhaseWidgetClassToUse = PhaseHUDWidgetClass;
		if (!PhaseWidgetClassToUse)
		{
			PhaseWidgetClassToUse = UIdiotsTogetherPhaseHUDWidget::StaticClass();
		}

		PhaseHUDWidgetInstance = CreateWidget<UIdiotsTogetherPhaseHUDWidget>(this, PhaseWidgetClassToUse);
		if (PhaseHUDWidgetInstance)
		{
			PhaseHUDWidgetInstance->AddToViewport(2);
		}
		else
		{
			UE_LOG(LogIdiotsTogetherUI, Error, TEXT("Failed to create phase HUD widget on %s"), *GetName());
		}
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

	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::One, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectBoss);
		InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectNerd);
		InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectMuscle);
		InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectIntern);
		InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevSelectRandom);
		InputComponent->BindKey(EKeys::Six, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevAdvancePhase);
		InputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevForceSuccess);
		InputComponent->BindKey(EKeys::Eight, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevForceFailure);
		InputComponent->BindKey(EKeys::Nine, IE_Pressed, this, &AIdiotsTogetherPlayerController::DevCompleteTestObjective);
		InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &AIdiotsTogetherPlayerController::ToggleDevCursor);
	}
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
	UE_LOG(LogIdiotsTogetherDev, Verbose, TEXT("Dev cursor %s"), bShowMouseCursor ? TEXT("ON") : TEXT("OFF"));
}

void AIdiotsTogetherPlayerController::ServerRequestCharacterSelection_Implementation(EIdiotsTogetherCharacterType Selection)
{
	UE_LOG(LogIdiotsTogetherPlayer, Verbose, TEXT("Character selection request %d from %s"),
		static_cast<int32>(Selection), *GetName());

	if (AIdiotsTogetherPlayerState* PS = GetPlayerState<AIdiotsTogetherPlayerState>())
	{
		PS->ServerSelectCharacter(Selection);
	}
	else
	{
		UE_LOG(LogIdiotsTogetherPlayer, Warning, TEXT("Character selection from %s ignored: no PlayerState"), *GetName());
	}
}

bool AIdiotsTogetherPlayerController::ServerRequestCharacterSelection_Validate(EIdiotsTogetherCharacterType Selection)
{
	return true;
}

void AIdiotsTogetherPlayerController::DevSelectBoss()
{
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Boss);
}

void AIdiotsTogetherPlayerController::DevSelectNerd()
{
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Nerd);
}

void AIdiotsTogetherPlayerController::DevSelectMuscle()
{
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Muscle);
}

void AIdiotsTogetherPlayerController::DevSelectIntern()
{
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Intern);
}

void AIdiotsTogetherPlayerController::DevSelectRandom()
{
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Random);
}

void AIdiotsTogetherPlayerController::DevAdvancePhase()
{
	ServerDevAdvancePhase();
}

void AIdiotsTogetherPlayerController::DevForceSuccess()
{
	ServerDevForceSuccess();
}

void AIdiotsTogetherPlayerController::DevForceFailure()
{
	ServerDevForceFailure();
}

void AIdiotsTogetherPlayerController::DevCompleteTestObjective()
{
	ServerDevCompleteTestObjective(TEXT("TEST_OBJECTIVE_01"));
}

void AIdiotsTogetherPlayerController::ServerDevAdvancePhase_Implementation()
{
	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("[%s] ServerDevAdvancePhase received"), *GetNetworkRoleString(this));
	if (AIdiotsTogetherGameModeBase* GM = GetWorld()->GetAuthGameMode<AIdiotsTogetherGameModeBase>())
	{
		GM->AdvancePhase();
	}
}

bool AIdiotsTogetherPlayerController::ServerDevAdvancePhase_Validate()
{
	return true;
}

void AIdiotsTogetherPlayerController::ServerDevForceSuccess_Implementation()
{
	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("[%s] ServerDevForceSuccess received"), *GetNetworkRoleString(this));
	if (AIdiotsTogetherGameModeBase* GM = GetWorld()->GetAuthGameMode<AIdiotsTogetherGameModeBase>())
	{
		GM->ForcePhaseSuccess();
	}
}

bool AIdiotsTogetherPlayerController::ServerDevForceSuccess_Validate()
{
	return true;
}

void AIdiotsTogetherPlayerController::ServerDevForceFailure_Implementation()
{
	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("[%s] ServerDevForceFailure received"), *GetNetworkRoleString(this));
	if (AIdiotsTogetherGameModeBase* GM = GetWorld()->GetAuthGameMode<AIdiotsTogetherGameModeBase>())
	{
		GM->ForcePhaseFailure();
	}
}

bool AIdiotsTogetherPlayerController::ServerDevForceFailure_Validate()
{
	return true;
}

void AIdiotsTogetherPlayerController::ServerDevCompleteTestObjective_Implementation(const FString& ObjectiveID)
{
	UE_LOG(LogIdiotsTogetherDev, Log, TEXT("[%s] ServerDevCompleteTestObjective received for %s"),
		*GetNetworkRoleString(this), *ObjectiveID);

	if (AIdiotsTogetherGameModeBase* GM = GetWorld()->GetAuthGameMode<AIdiotsTogetherGameModeBase>())
	{
		GM->CompleteObjective(ObjectiveID);
	}
}

bool AIdiotsTogetherPlayerController::ServerDevCompleteTestObjective_Validate(const FString& ObjectiveID)
{
	return true;
}

