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

	UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] PlayerController BeginPlay: Name=%s, Class=%s, IsLocal=%s, PossessedPawn=%s (Class=%s)"),
		*GetName(),
		*GetClass()->GetName(),
		IsLocalController() ? TEXT("TRUE") : TEXT("FALSE"),
		GetPawn() ? *GetPawn()->GetName() : TEXT("NULL"),
		GetPawn() ? *GetPawn()->GetClass()->GetName() : TEXT("NONE"));

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] Enhanced Input Subsystem initialized for PlayerController."));
	}

	if (IsLocalController())
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] PlayerController SetInputMode FInputModeGameOnly enabled."));
	}

	if (IsLocalController() && InteractionWidgetClass)
	{
		InteractionWidgetInstance = CreateWidget<UIdiotsTogetherInteractionWidget>(this, InteractionWidgetClass);
		if (InteractionWidgetInstance)
		{
			InteractionWidgetInstance->AddToViewport(10);
			InteractionWidgetInstance->HidePrompt();
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] InteractionWidgetInstance created and added to viewport."));
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
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A DEV UI] M3A DEV UI CREATED on %s"), *GetName());
		}
		else
		{
			UE_LOG(LogIdiotsTogether, Error, TEXT("[M3A DEV UI] FAILED to create DevCharSelectWidgetInstance on %s"), *GetName());
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
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3C UI] PhaseHUDWidgetInstance created and added to viewport (ZOrder=2) on %s"), *GetName());
		}
		else
		{
			UE_LOG(LogIdiotsTogether, Error, TEXT("[M3C UI] FAILED to create PhaseHUDWidgetInstance on %s"), *GetName());
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
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[DEV INPUT] Bound keys 1-5 & Tab for character selection, keys 6-8 for phase debug, key 9 for objective completion debug."));
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
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[DEV INPUT] Toggled mouse cursor: %s"), bShowMouseCursor ? TEXT("ON") : TEXT("OFF"));
}

void AIdiotsTogetherPlayerController::ServerRequestCharacterSelection_Implementation(EIdiotsTogetherCharacterType Selection)
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A SERVER SELECT] M3A SERVER SELECT: RequestedType=%d from PC=%s"),
		static_cast<int32>(Selection), *GetName());

	if (AIdiotsTogetherPlayerState* PS = GetPlayerState<AIdiotsTogetherPlayerState>())
	{
		PS->ServerSelectCharacter(Selection);
	}
	else
	{
		UE_LOG(LogIdiotsTogether, Warning, TEXT("[SERVER RPC] PlayerState is NULL on PC %s during ServerRequestCharacterSelection!"), *GetName());
	}
}

bool AIdiotsTogetherPlayerController::ServerRequestCharacterSelection_Validate(EIdiotsTogetherCharacterType Selection)
{
	return true;
}

void AIdiotsTogetherPlayerController::DevSelectBoss()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A INPUT] M3A INPUT: 1 (Boss)"));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A SELECT REQUEST] M3A SELECT REQUEST: RequestedType=Boss (1)"));
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Boss);
}

void AIdiotsTogetherPlayerController::DevSelectNerd()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A INPUT] M3A INPUT: 2 (Nerd)"));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A SELECT REQUEST] M3A SELECT REQUEST: RequestedType=Nerd (2)"));
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Nerd);
}

void AIdiotsTogetherPlayerController::DevSelectMuscle()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A INPUT] M3A INPUT: 3 (Muscle)"));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A SELECT REQUEST] M3A SELECT REQUEST: RequestedType=Muscle (3)"));
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Muscle);
}

void AIdiotsTogetherPlayerController::DevSelectIntern()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A INPUT] M3A INPUT: 4 (Intern)"));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A SELECT REQUEST] M3A SELECT REQUEST: RequestedType=Intern (4)"));
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Intern);
}

void AIdiotsTogetherPlayerController::DevSelectRandom()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A INPUT] M3A INPUT: 5 (Random)"));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3A SELECT REQUEST] M3A SELECT REQUEST: RequestedType=Random (5)"));
	ServerRequestCharacterSelection(EIdiotsTogetherCharacterType::Random);
}

void AIdiotsTogetherPlayerController::DevAdvancePhase()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] DevAdvancePhase pressed"), *GetNetworkRoleString(this));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] Sending ServerDevAdvancePhase RPC"), *GetNetworkRoleString(this));
	ServerDevAdvancePhase();
}

void AIdiotsTogetherPlayerController::DevForceSuccess()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] DevForceSuccess pressed"), *GetNetworkRoleString(this));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] Sending ServerDevForceSuccess RPC"), *GetNetworkRoleString(this));
	ServerDevForceSuccess();
}

void AIdiotsTogetherPlayerController::DevForceFailure()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] DevForceFailure pressed"), *GetNetworkRoleString(this));
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] Sending ServerDevForceFailure RPC"), *GetNetworkRoleString(this));
	ServerDevForceFailure();
}

void AIdiotsTogetherPlayerController::DevCompleteTestObjective()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D/DEV][%s] DevCompleteTestObjective pressed (Key 9)"), *GetNetworkRoleString(this));
	ServerDevCompleteTestObjective(TEXT("TEST_OBJECTIVE_01"));
}

void AIdiotsTogetherPlayerController::ServerDevAdvancePhase_Implementation()
{
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] ServerDevAdvancePhase received"), *GetNetworkRoleString(this));
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
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] ServerDevForceSuccess received"), *GetNetworkRoleString(this));
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
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3B][%s] ServerDevForceFailure received"), *GetNetworkRoleString(this));
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
	UE_LOG(LogIdiotsTogether, Warning, TEXT("[M3D/DEV][%s] ServerDevCompleteTestObjective received for ObjID=%s"),
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

