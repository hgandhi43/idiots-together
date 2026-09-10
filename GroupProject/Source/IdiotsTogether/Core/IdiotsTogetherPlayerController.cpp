#include "IdiotsTogetherPlayerController.h"
#include "EnhancedInputSubsystemInterface.h"
#include "EnhancedInputSubsystems.h"
#include "IdiotsTogether/UI/IdiotsTogetherInteractionWidget.h"
#include "IdiotsTogether/IdiotsTogether.h"

AIdiotsTogetherPlayerController::AIdiotsTogetherPlayerController()
{
	bShowMouseCursor = false;
	DefaultMouseCursor = EMouseCursor::Default;
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
		InteractionWidgetInstance = CreateWidget<UIIdiotsTogetherInteractionWidget>(this, InteractionWidgetClass);
		if (InteractionWidgetInstance)
		{
			InteractionWidgetInstance->AddToViewport(10);
			InteractionWidgetInstance->HidePrompt();
			UE_LOG(LogIdiotsTogether, Warning, TEXT("[RUNTIME DIAGNOSTIC] InteractionWidgetInstance created and added to viewport."));
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
}
